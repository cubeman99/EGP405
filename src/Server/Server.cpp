#include "Server.h"
#include <GameLib/util/Timing.h>
#include <GameLib/math/MathLib.h>
#include <GameLib/math/Polygonf.h>

using namespace RakNet;


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


Server::Server()
{
}

Server::~Server()
{
}

int Server::Initialize()
{
	// Configure & initialize the game world.
	GameConfig config;
	config.maxPlayers				= 20;
	config.scorePauseSeconds		= 1.0f;
	config.view.floorY				= 350;
	config.view.width				= 750;
	config.view.height				= 500;
	config.net.width				= 4;
	config.net.height				= 41;
	config.net.depthBelowGround		= 4;
	config.ball.radius				= 11;
	config.ball.gravity				= 0.22f * 60.0f * 60.0f;
	config.ball.maxSpeed			= 10 * 60.0f;
	config.ball.serveHeight			= 130;
	config.ball.maxBounceXVelocity	= 15 * 60;
	config.ball.maxBounceYVelocity	= 22 * 60;
	config.slime.radius				= 37;
	config.slime.gravity			= 0.5f * 60.0f * 60.0f;
	config.slime.jumpSpeed			= 9 * 60.0f;
	config.slime.moveSpeed			= 4.5f * 60.0f;

	m_gameWorld.Initialize(config);
	m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_PLAYERS);
	m_gameWorld.PositionBallAboveNet();

	printf("Starting the server on port %d.\n", SERVER_PORT);
	if (!m_networkManager.Initialize(SERVER_PORT, config.maxPlayers, this))
	{
		printf("Error: failure to start the server.\n");
		return -1;
	}

	return 0;
}

int Server::Run()
{
	float m_tickRate = 20.0f;

	float m_currentTickRate = m_tickRate;
	float m_frameTime = 1.0f / m_tickRate;
	bool m_isRunning = true;

	double startTime = Time::GetTime();
	double lastTime = startTime;
	double nextTickTime = startTime;
	double frameTime = 1.0 / m_tickRate;
	int    frames = 0;

	double newTime;

	while (m_isRunning)
	{
		newTime = Time::GetTime();

		// Update tick rate tracker.
		if (newTime > startTime + 1.0)
		{
			m_currentTickRate = frames / (float) (newTime - startTime);
			startTime += 1.0;
			frames = 0;
		}

		// Tick at a constant rate.
		if (newTime >= nextTickTime + frameTime)
		{
			Tick(m_frameTime);
			frames++;

			nextTickTime += frameTime;
			if (newTime > nextTickTime + frameTime)
				nextTickTime = newTime + frameTime;
		}
	}

	return 0;
}

void Server::Tick(float timeDelta)
{
	Ball& ball = m_gameWorld.GetBall();

	// Process any queued packets.
	m_networkManager.ReceivePackets();

	// Process input and simulate movement for each client.
	for (auto it = m_networkManager.clients_begin(); it != m_networkManager.clients_end(); it++)
	{
		MoveList& moveList = it->second->GetUnprocessedMoveList();
		Slime* player = m_gameWorld.GetPlayer(it->second->GetPlayerId());

		// Process movement when in gameplay.
		if (player != NULL &&
			(m_gameWorld.GetState() == GameWorld::STATE_GAMEPLAY ||
			m_gameWorld.GetState() == GameWorld::STATE_WAITING_FOR_PLAYERS))
		{
			for (int i = 0; i < moveList.GetMoveCount(); i++)
			{
				Move& unprocessedMove = moveList[i];
				const InputState& currentState = unprocessedMove.GetInputState();
				float moveTimeDelta = unprocessedMove.GetDeltaTime();

				m_gameWorld.ProcessPlayerInput(player, currentState, moveTimeDelta);
				m_gameWorld.SimulatePlayerMovement(player, moveTimeDelta);
			}
		}

		moveList.Clear();
	}

	// Update game states.
	if (m_gameWorld.GetState() == GameWorld::STATE_GAMEPLAY)
	{
		// Simulate ball physics.
		m_gameWorld.SimulateBallMovement(timeDelta);

		// Check for a score - the ball touches the ground.
		if (ball.GetPosition().y + ball.GetRadius() >= m_gameWorld.GetConfig().view.floorY)
		{
			ball.SetPosition(Vector2f(ball.GetPosition().x,
				m_gameWorld.GetConfig().view.floorY - ball.GetRadius()));
			OnTeamScore(ball.GetPosition().x < m_gameWorld.GetConfig().view.width * 0.5f ? 1 : 0);
		}
	}
	else if (m_gameWorld.GetState() == GameWorld::STATE_WAITING_FOR_SERVE)
	{
		// Pause the game until the next round.
		m_serveDelayTimer -= timeDelta;

		if (m_serveDelayTimer <= 0.0f)
		{
			BeginNewRound();
		}
	}
	else if (m_gameWorld.GetState() == GameWorld::STATE_WAITING_FOR_PLAYERS)
	{
		// Position the ball floating motionless above the net.
		m_gameWorld.PositionBallAboveNet();
	}

	// Send state info back to the clients.
	for (auto it = m_networkManager.clients_begin(); it != m_networkManager.clients_end(); it++)
	{
		ClientProxy* client = it->second;

		// Write the client's info.
		BitStream bsOut;
		bsOut.Write((MessageID) PacketType::UPDATE_TICK);
		m_networkManager.WriteLastMoveTimestampIfDirty(bsOut, it->second);
		bsOut.Write(ball.GetPosition());
		bsOut.Write(ball.GetVelocity());
		for (auto it2 = m_gameWorld.players_begin(); it2 != m_gameWorld.players_end(); it2++)
		{
			Slime* slime = it2->second;

			if (slime->HasJoinedGame())
			{
				bsOut.Write(slime->GetPlayerId());
				slime->SerializeDynamics(bsOut, m_gameWorld.GetConfig());
			}
		}
		bsOut.Write((int) -1);
		m_networkManager.Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, client);
	}
}

void Server::BeginNewRound()
{
	// Position the players and ball for a serve.
	m_gameWorld.PositionBallForServe(m_servingTeamIndex);
	m_gameWorld.PositionPlayersForServe();

	if (AreBothTeamsReady())
	{
		// Begin playing the new round.
		m_gameWorld.SetState(GameWorld::STATE_GAMEPLAY);

		// Tell the clients that the round has begun, letting them move around again.
		BitStream bsOut;
		bsOut.Write((MessageID) PacketType::TEAM_SERVE);
		m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED);
	}
	else
	{
		// If there isn't one person-per-team, then go back to 
		// the waiting-fo-players state.
		BeginWaitingForPlayers();
		
		// Tell the clients that we are waiting for more players.
		BitStream bsOut;
		bsOut.Write((MessageID) PacketType::WAITING_FOR_PLAYERS);
		m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED);
	}
}

void Server::BeginWaitingForPlayers()
{
	m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_PLAYERS);
	m_gameWorld.PositionBallAboveNet();
}

void Server::OnTeamScore(int teamIndex)
{
	m_servingTeamIndex = teamIndex;
	m_serveDelayTimer = m_gameWorld.GetConfig().scorePauseSeconds;
	m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_SERVE);

	// Tell the clients that a team has scored, freezing 
	// their movement for a duration.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::TEAM_SCORED);
	bsOut.Write(m_servingTeamIndex);
	m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED);
}

void Server::OnClientConnect(ClientProxy* client)
{
	printf("Player ID %d has connected.\n", client->GetPlayerId());

	// Reply with an ACCEPT_CONNECTION packet, containing the connected player's
	// assigned player ID as well as a snapshot of the current game state: score
	// and a list of joined players.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::ACCEPT_CONNECTION);
	bsOut.Write(client->GetPlayerId());
	bsOut.Write(m_gameWorld.GetConfig());
	bsOut.Write(m_gameWorld.GetTeam(0).GetScore());
	bsOut.Write(m_gameWorld.GetTeam(1).GetScore());
	for (auto it = m_gameWorld.players_begin(); it != m_gameWorld.players_end(); it++)
	{
		Slime* slime = it->second;
		if (slime->HasJoinedGame())
		{
			bsOut.Write(slime->GetPlayerId());
			bsOut.Write(slime->GetTeamIndex());
			bsOut.Write(slime->GetColorIndex());
		}
	}
	bsOut.Write((int) -1);
	m_networkManager.Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, client);
}

void Server::OnClientDisconnect(ClientProxy* client)
{
	printf("Player ID %d has diconnected.\n", client->GetPlayerId());

	// Find the player from the packet's RakNet GUID.
	Slime* player = m_gameWorld.GetPlayer(client->GetPlayerId());

	// Remove the player from memory.
	m_gameWorld.RemovePlayer(client->GetPlayerId());

	// Tell the other clients a player has disconnected.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::PLAYER_LEFT);
	bsOut.Write(client->GetPlayerId());
	m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, client);
}

void Server::OnClientEvent(ClientProxy* client, NetworkEventID eventId, RakNet::BitStream& inStream)
{
	switch (eventId)
	{
	case PacketType::JOIN_TEAM:
		OnPlayerJoinTeam(client, inStream);
		break;
	}
}

void Server::OnPlayerJoinTeam(ClientProxy* client, RakNet::BitStream& inStream)
{
	int colorIndex;
	int teamIndex;
	
	// Create a new player entity.
	Slime* player = m_gameWorld.CreatePlayer(client->GetPlayerId());

	// Read the packet.
	inStream.Read(teamIndex);
	inStream.Read(colorIndex);

	// Update the player info.
	player->SetTeamIndex(teamIndex);
	player->SetColorIndex(colorIndex);
	player->SetJoinedGame(true);
	player->SetPosition(m_gameWorld.GetTeam(teamIndex).GetPlayerSpawnPosition());

	// Tell the other clients that a player has joined.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::PLAYER_JOINED);
	bsOut.Write(client->GetPlayerId());
	bsOut.Write(teamIndex);
	bsOut.Write(colorIndex);
	m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, client);
	
	printf("Player ID %d has joined team %d!\n", client->GetPlayerId(), teamIndex);

	// Check if there is at least one ready player per team.
	// If so, then start the match.
	if (AreBothTeamsReady())
	{
		m_servingTeamIndex = 0;
		m_serveDelayTimer = m_gameWorld.GetConfig().scorePauseSeconds;
		m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_SERVE);
		m_gameWorld.PositionPlayersForServe();
		m_gameWorld.PositionBallForServe(m_servingTeamIndex);
		printf("There are enough players to start a match!\n");
	}
}

bool Server::AreBothTeamsReady()
{
	// Check if there is at least one ready player per team.
	bool isTeamReady[2] = { false, false };
	for (auto it = m_gameWorld.players_begin(); it != m_gameWorld.players_end(); it++)
	{
		if (it->second->HasJoinedGame())
			isTeamReady[it->second->GetTeamIndex()] = true;
	}

	return (isTeamReady[0] && isTeamReady[1]);
}

