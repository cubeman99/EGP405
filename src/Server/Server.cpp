#include "Server.h"
#include <GameLib/util/Timing.h>
#include <GameLib/math/MathLib.h>
#include <GameLib/math/Polygonf.h>

using namespace RakNet;


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


Server::Server()
{
	m_gameConfig.maxPlayers				= 20;
	m_gameConfig.scorePauseSeconds		= 1.0f;
	m_gameConfig.view.floorY			= 350;
	m_gameConfig.view.width				= 750;
	m_gameConfig.view.height			= 500;
	m_gameConfig.net.width				= 4;
	m_gameConfig.net.height				= 41;
	m_gameConfig.net.depthBelowGround	= 4;
	m_gameConfig.ball.radius			= 11;
	m_gameConfig.ball.gravity			= 0.22f;
	m_gameConfig.ball.maxSpeed			= 10;
	m_gameConfig.ball.serveHeight		= 130;
	m_gameConfig.slime.radius			= 37;
	m_gameConfig.slime.gravity			= 0.5f;
	m_gameConfig.slime.jumpSpeed		= 9;
	m_gameConfig.slime.moveSpeed		= 4.5f;
}

Server::~Server()
{
	// Delete all players.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		delete it->second;
	}
}

int Server::Initialize()
{
	printf("Starting the server on port %d.\n", SERVER_PORT);
	
	if (!m_networkManager.Initialize(SERVER_PORT, m_gameConfig.maxPlayers, this))
	{
		printf("Error: failure to start the server.\n");
		return -1;
	}

	m_state = STATE_WAIT_FOR_PLAYERS;
	m_serveDelayTimer = 0.0f;
	m_teams[0] = Team(0, "1", Rect2f(0.0f, 0.0f,
		(m_gameConfig.view.width - m_gameConfig.net.width) * 0.5f, m_gameConfig.view.floorY), m_gameConfig);
	m_teams[1] = Team(1, "2", Rect2f((m_gameConfig.view.width + m_gameConfig.net.width) * 0.5f,
		0.0f, (m_gameConfig.view.width - m_gameConfig.net.width) * 0.5f, m_gameConfig.view.floorY), m_gameConfig);
	m_ball = Ball(m_gameConfig.ball.radius, Vector2f(m_gameConfig.view.width * 0.25f,
			m_gameConfig.view.floorY - m_gameConfig.ball.serveHeight));

	return 0;
}

int Server::Run()
{
	float m_tickRate = 60.0f;

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
	// Process any queued packets.
	m_networkManager.ReceivePackets();

	// Process input and simulate movement for each player.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Slime* player = it->second;
		ClientProxy* client = m_networkManager.GetClientProxy(player->GetPlayerId());

		MoveList& moveList = client->GetUnprocessedMoveList();

		for (int i = 0; i < moveList.GetMoveCount(); i++)
		{
			Move& unprocessedMove = moveList[i];
			const InputState& currentState = unprocessedMove.GetInputState();
			float deltaTime = unprocessedMove.GetDeltaTime();

			//float xVelocity = currentState.GetDesiredHorizontalDelta() *
				//deltaTime * m_gameConfig.slime.moveSpeed;
			//float xVelocity = currentState.GetDesiredHorizontalDelta() * 4.5f;
			//player->SetPosition(player->GetPosition() + Vector2f(xVelocity, 0.0f));

			ProcessPlayerInput(player, currentState, deltaTime);
			SimulatePlayerMovement(player, deltaTime);
		}

		moveList.Clear();
	}

	// Update game states.
	if (m_state == STATE_PLAY_GAME)
	{
		// Simulate ball physics.
		UpdateBall();
	}
	else if (m_state == STATE_WAIT_FOR_SERVE)
	{
		m_serveDelayTimer -= timeDelta;

		if (m_serveDelayTimer <= 0.0f)
		{
			BeginNewRound();
		}
	}
	else if (m_state == STATE_WAIT_FOR_PLAYERS)
	{
		// Position the ball floating statically above the net.
		m_ball.SetPosition(Vector2f(m_gameConfig.view.width * 0.5f,
			m_gameConfig.view.floorY - m_gameConfig.ball.serveHeight));
	}
	
	// Broadcast a message to clients containing ball and player state info.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::UPDATE_TICK);
	bsOut.Write(m_ball.GetPosition());
	bsOut.Write(m_ball.GetVelocity());
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Slime* slime = it->second;

		if (slime->HasJoinedGame())
		{
			bsOut.Write(slime->GetPlayerId());
			slime->SerializeDynamics(bsOut, m_gameConfig);
		}
	}
	bsOut.Write((int) -1);
	m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, UNRELIABLE);
}

void Server::ProcessPlayerInput(Slime* player, const InputState& currentState, float deltaTime)
{
	Vector2f velocity = player->GetVelocity();
	Vector2f position = player->GetPosition();

	velocity.x = currentState.GetDesiredHorizontalDelta() * m_gameConfig.slime.moveSpeed;

	if (currentState.IsJumping() && position.y >= m_gameConfig.view.floorY)
		velocity.y = -m_gameConfig.slime.jumpSpeed;
	
	player->SetVelocity(velocity);
}

void Server::SimulatePlayerMovement(Slime* player, float deltaTime)
{
	Vector2f position = player->GetPosition();
	Vector2f velocity = player->GetVelocity();
	Team* team = &m_teams[player->GetTeamIndex()];
	float minX = team->GetPlayRegion().GetLeft();
	float maxX = team->GetPlayRegion().GetRight();
	float radius = player->GetRadius();

	// Integrate gravitational force.
	velocity.y += (m_gameConfig.slime.gravity * 60.0f) * deltaTime;
	
	// Collide with floor and walls.
	if (position.y >= m_gameConfig.view.floorY)
	{
		position.y = m_gameConfig.view.floorY;
		if (velocity.y > 0.0f)
			velocity.y = 0.0f;
	}
	if (position.x - radius <= minX)
	{
		position.x = minX + radius;
		if (velocity.x < 0.0f)
		velocity.x = 0.0f;
	}
	else if (position.x + radius >= maxX)
	{
		position.x = maxX - radius;
		if (velocity.x > 0.0f)
			velocity.x = 0.0f;
	}

	// Integrate velocity.
	position += (velocity * 60.0f) * deltaTime;

	player->SetPosition(position);
	player->SetVelocity(velocity);
}

void Server::BeginNewRound()
{
	// Position the ball for the serving team.
	m_ball.SetPosition(m_teams[m_servingTeamIndex].GetBallServePosition());
	m_ball.SetVelocity(Vector2f::ZERO);

	// Position the players in the middle of their areas.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
		it->second->SetPosition(m_teams[it->second->GetTeamIndex()].GetPlayerSpawnPosition());

	// Tell the clients that the round has begun, letting them 
	// move around again.
	BitStream bsOut;
	bsOut.Write((MessageID) PacketType::TEAM_SERVE);
	m_networkManager.Broadcast(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED);

	if (AreBothTeamsReady())
	{
		// Begin playing the new round.
		m_state = STATE_PLAY_GAME;
	}
	else
	{
		// If there isn't one person-per-team, then go back to 
		// the waiting-fo-players state.
		BeginWaitingForPlayers();
	}
}

void Server::BeginWaitingForPlayers()
{
	m_state = STATE_WAIT_FOR_PLAYERS;

	// Position the ball floating statically above the net.
	m_ball.SetPosition(Vector2f(m_gameConfig.view.width * 0.5f,
		m_gameConfig.view.floorY - m_gameConfig.ball.serveHeight));
}

void Server::OnTeamScore(int teamIndex)
{
	m_servingTeamIndex = teamIndex;

	m_state = STATE_WAIT_FOR_SERVE;
	m_serveDelayTimer = m_gameConfig.scorePauseSeconds;

	// Tell the clients that a team has scored, freezing 
	// their movement for a duration.
	BitStream bsOut;
	bsOut.Write((MessageID)PacketType::TEAM_SCORED);
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
	bsOut.Write(m_gameConfig);
	bsOut.Write(m_teams[0].GetScore());
	bsOut.Write(m_teams[1].GetScore());
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
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
	Slime* player = m_players[client->GetPlayerId()];

	// Remove the player from memory.
	delete player;
	m_players.erase(client->GetPlayerId());

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
	{
		OnPlayerJoinTeam(client, inStream);
		break;
	}
	//case PacketType::CLIENT_UPDATE_TICK:
	//{
	//	// Read the packet.
	//	BitStream bsIn(packet->data, packet->length, false);
	//	bsIn.IgnoreBytes(sizeof(MessageID));
	//	int playerId = m_guidToPlayerMap[packet->guid];
	//	Slime* player = m_players[playerId];
	//	player->DeserializeDynamics(bsIn, m_gameConfig);
	//	break;
	//}
	}
}

void Server::OnPlayerJoinTeam(ClientProxy* client, RakNet::BitStream& inStream)
{
	int colorIndex;
	int teamIndex;
	
	// Create a new player entity.
	Slime* player = new Slime(client->GetPlayerId(), 0, 0, m_gameConfig.slime.radius);
	m_players[client->GetPlayerId()] = player;

	// Read the packet.
	inStream.Read(teamIndex);
	inStream.Read(colorIndex);

	// Update the player info.
	player->SetTeamIndex(teamIndex);
	player->SetColorIndex(colorIndex);
	player->SetJoinedGame(true);
	player->SetPosition(m_teams[teamIndex].GetPlayerSpawnPosition());

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
		m_state = STATE_WAIT_FOR_SERVE;
		m_serveDelayTimer = m_gameConfig.scorePauseSeconds;
		m_servingTeamIndex = 0;
		for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
			it->second->SetPosition(m_teams[it->second->GetTeamIndex()].GetPlayerSpawnPosition());
		printf("There are enough players to start a match!\n");
	}
}

void Server::UpdateBall()
{
	Vector2f position = m_ball.GetPosition();
	Vector2f velocity = m_ball.GetVelocity();

	velocity.y += m_gameConfig.ball.gravity;
	position += velocity;

	// Collide with slimes.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Slime& slime = *it->second;

		if (!slime.HasJoinedGame())
			continue;

		Vector2f slimePos = slime.GetPosition();
		Vector2f slimeVel = slime.GetVelocity();
		Vector2f relPosition = position - slimePos;
		Vector2f relVelocity = velocity - slimeVel;
		float distSqr = relPosition.LengthSquared();
		float radiusSum = m_ball.GetRadius() + slime.GetRadius();

		if (relPosition.y < 0.0f && distSqr <= (radiusSum * radiusSum) && distSqr > 4.0f)
		{
			float dist = Math::Sqrt(distSqr);
			Vector2f slime2ball = relPosition / dist;
			float proj = slime2ball.Dot(relVelocity);
			position = slimePos + (slime2ball * radiusSum);

			if (proj <= 0.0f)
			{
				velocity += slimeVel - (2.0f * slime2ball * proj);
				velocity.y -= m_gameConfig.ball.gravity * 0.5f;

				if (velocity.x < -15)
					velocity.x = -15;
				if (velocity.x > 15)
					velocity.x = 15;
				if (velocity.y < -22)
					velocity.y = -22;
				if (velocity.y > 22)
					velocity.y = 22;
			}
		}
	}

	// Collide ball with the net.
	{
		float radius = m_ball.GetRadius();
		Vector2f v1(m_gameConfig.view.width * 0.5f, m_gameConfig.view.floorY -
			m_gameConfig.net.height + m_gameConfig.net.depthBelowGround);
		Vector2f v2(m_gameConfig.view.width * 0.5f, m_gameConfig.view.floorY +
			m_gameConfig.net.depthBelowGround);

		// Determine which voronoi region of the edge center of circle lies within.
		float dot1 = (position - v1).Dot(v2 - v1);
		float dot2 = (position - v2).Dot(v1 - v2);

		// Closest to v1.
		if (dot1 <= 0.0f)
		{
			if (position.DistToSqr(v1) < radius * radius)
			{
				Vector2f n = position - v1;
				n.Normalize();
				position = v1 + (radius * n);
				if (velocity.Dot(n) < 0.0f)
					velocity = velocity - (2.0f * velocity.Dot(n) * n);
				printf("Ball collided with top of net\n");
			}
		}
		// Closest to v2.
		else if (dot2 <= 0.0f)
		{
			if (position.DistToSqr(v2) < radius * radius)
			{
				Vector2f n = position - v2;
				n.Normalize();
				position = v2 + (radius * n);
				if (velocity.Dot(n) < 0.0f)
					velocity = velocity - (2.0f * velocity.Dot(n) * n);
			}
		}
		else if (Math::Abs(position.x - v1.x) < radius)
		{
			if (position.x < v1.x)
			{
				position.x = v1.x - radius;
				if (velocity.x > 0.0f)
					velocity.x = -velocity.x;
			}
			else
			{
				position.x = v1.x + radius;
				if (velocity.x < 0.0f)
					velocity.x = -velocity.x;
			}
		}
	}

	// Collide with the floor and walls.
	if (position.x - m_ball.GetRadius() <= 0.0f)
	{
		position.x = m_ball.GetRadius();
		if (velocity.x < 0.0f)
			velocity.x = -velocity.x;
	}
	else if (position.x + m_ball.GetRadius() >= m_gameConfig.view.width)
	{
		position.x = m_gameConfig.view.width - m_ball.GetRadius();
		if (velocity.x > 0.0f)
			velocity.x = -velocity.x;
	}
	if (position.y + m_ball.GetRadius() >= m_gameConfig.view.floorY)
	{
		position.y = m_gameConfig.view.floorY - m_ball.GetRadius();
		velocity = Vector2f::ZERO;
		OnTeamScore(position.x < m_gameConfig.view.width * 0.5f ? 1 : 0);
		//if (velocity.y > 0.0f)
			//velocity.y = -velocity.y;
	}

	// Limit max speed.
	float velLength = velocity.Length();
	if (velLength > m_gameConfig.ball.maxSpeed)
		velocity *= m_gameConfig.ball.maxSpeed / velLength;

	m_ball.SetPosition(position);
	m_ball.SetVelocity(velocity);
}

bool Server::AreBothTeamsReady()
{
	// Check if there is at least one ready player per team.
	bool isTeamReady[2] = { false, false };
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		if (it->second->HasJoinedGame())
			isTeamReady[it->second->GetTeamIndex()] = true;
	}

	return (isTeamReady[0] && isTeamReady[1]);
}

