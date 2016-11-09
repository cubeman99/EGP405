#include "Server.h"
#include <GameLib/util/Timing.h>
#include <GameLib/math/MathLib.h>
#include <GameLib/math/Polygonf.h>

using namespace RakNet;


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


Server::Server()
{
	m_peerInterface = RakPeerInterface::GetInstance();

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
	// Setup the server socket.
	printf("Starting the server on port %d.\n", SERVER_PORT);
	SocketDescriptor sd(SERVER_PORT, 0);
	StartupResult result = m_peerInterface->Startup(m_gameConfig.maxPlayers, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(m_gameConfig.maxPlayers); // Max number of players.
	m_peerInterface->SetOccasionalPing(true);

	if (result != StartupResult::RAKNET_STARTED)
	{
		printf("Error: failure to start the RakNet peer interface.\n");
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
			ReceivePackets();
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
	// Update game states.
	if (m_state == STATE_PLAY_GAME)
	{
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
	bsOut.Write((MessageID) GameMessages::UPDATE_TICK);
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
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE,
		0, RakNet::UNASSIGNED_RAKNET_GUID, true);
}

void Server::BeginNewRound()
{
	// Position the ball for the serving team.
	m_ball.SetPosition(m_teams[m_servingTeamIndex].GetBallServePosition());
	m_ball.SetVelocity(Vector2f::ZERO);

	// Tell the clients that the round has begun, letting them 
	// move around again.
	BitStream bsOut;
	bsOut.Write((MessageID)GameMessages::TEAM_SERVE);
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, RakNet::UNASSIGNED_RAKNET_GUID, true);

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
	bsOut.Write((MessageID)GameMessages::TEAM_SCORED);
	bsOut.Write(m_servingTeamIndex);
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, RakNet::UNASSIGNED_RAKNET_GUID, true);
}

void Server::OnPlayerConnect(RakNet::Packet* connectionPacket)
{
	// Reply with an ACCEPT_CONNECTION packet, containing the connected player's
	// assigned player ID as well as the current game state: score and a list
	// of joined players.
	BitStream bsOut;
	bsOut.Write((MessageID) GameMessages::ACCEPT_CONNECTION);
	bsOut.Write(m_playerIdCounter);
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
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
		RELIABLE_ORDERED, 0, connectionPacket->systemAddress, false);

	// Create a new player entity.
	int playerId = GetNewPlayerId();
	m_guidToPlayerMap[connectionPacket->guid] = playerId;
	m_players[playerId] = new Slime(playerId, 0, 0, m_gameConfig.slime.radius);
	m_players[playerId]->SetPlayerId(playerId);

	printf("Player ID %d has connected.\n", playerId);
}

void Server::OnPlayerDisconnect(RakNet::Packet* disconnectionPacket)
{
	// Find the player from the packet's RakNet GUID.
	int playerId = m_guidToPlayerMap[disconnectionPacket->guid];
	Slime* player = m_players[playerId];

	// Remove the player.
	delete player;
	m_players.erase(playerId);
	m_guidToPlayerMap.erase(disconnectionPacket->guid);

	// Tell the clients a player has disconnected.
	BitStream bsOut;
	bsOut.Write((MessageID) GameMessages::PLAYER_LEFT);
	bsOut.Write(playerId);
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED,
		0, disconnectionPacket->systemAddress, true);

	printf("Player ID %d has diconnected.\n", playerId);
}

void Server::OnPlayerJoinGame(RakNet::Packet* joinPacket)
{
	int colorIndex;
	int teamIndex;

	// Read the packet.
	BitStream bsIn(joinPacket->data, joinPacket->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(teamIndex);
	bsIn.Read(colorIndex);

	// Update the player info.
	int playerId = m_guidToPlayerMap[joinPacket->guid];
	Slime* player = m_players[playerId];
	player->SetTeamIndex(teamIndex);
	player->SetColorIndex(colorIndex);
	player->SetJoinedGame(true);

	// Tell the clients that a player has joined.
	BitStream bsOut;
	bsOut.Write((MessageID) GameMessages::PLAYER_JOINED);
	bsOut.Write(playerId);
	bsOut.Write(teamIndex);
	bsOut.Write(colorIndex);
	m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
		RELIABLE_ORDERED, 0, joinPacket->systemAddress, true);
	
	printf("Player ID %d has joined team %d!\n", playerId, teamIndex);

	// Check if there is at least one ready player per team.
	// If so, then start the match.
	if (AreBothTeamsReady())
	{
		m_state = STATE_WAIT_FOR_SERVE;
		m_serveDelayTimer = m_gameConfig.scorePauseSeconds;
		m_servingTeamIndex = 0;
		printf("There are enough players to start a match!\n");
	}
}

int Server::GetNewPlayerId()
{
	m_playerIdCounter++;
	return (m_playerIdCounter - 1);
}

void Server::ReceivePackets()
{
	Packet *packet;

	for (packet = m_peerInterface->Receive(); packet != NULL;
		m_peerInterface->DeallocatePacket(packet),
		packet = m_peerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // A player has connected.
		case ID_NEW_INCOMING_CONNECTION:
		{
			OnPlayerConnect(packet);
			break;
		}
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		case ID_REMOTE_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
		case GameMessages::DISCONNECTED:
		{
			OnPlayerDisconnect(packet);
			break;
		}
		case GameMessages::JOIN_GAME:
		{
			OnPlayerJoinGame(packet);
			break;
		}
		case GameMessages::CLIENT_UPDATE_TICK:
		{
			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			int playerId = m_guidToPlayerMap[packet->guid];
			Slime* player = m_players[playerId];
			player->DeserializeDynamics(bsIn, m_gameConfig);
			break;
		}
		}
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
				printf("Ball collide with top of net\n");
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
				printf("Collide bot\n");
			}
		}
		else if (Math::Abs(position.x - v1.x) < radius)
		{
			if (position.x < v1.x)
			{
				position.x = v1.x - radius;
				if (velocity.x > 0.0f)
					velocity.x = -velocity.x;
				printf("Collide left\n");
			}
			else
			{
				position.x = v1.x + radius;
				if (velocity.x < 0.0f)
					velocity.x = -velocity.x;
				printf("Collide right\n");
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

