#include "Server.h"
#include <GameLib/util/Timing.h>
#include <GameLib/math/MathLib.h>

using namespace RakNet;


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


Server::Server() :
	m_socketDescriptor(SERVER_PORT, 0)
{
	m_peerInterface = RakPeerInterface::GetInstance();

	m_ball = Ball(m_config.BALL_COLOR, m_config.BALL_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.25f,
			m_config.FLOOR_Y - m_config.BALL_SERVE_HEIGHT));
}

Server::~Server()
{
	// Delete all players.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		delete it->second;
	}
}

int Server::Run()
{
	// Setup the server socket.
	printf("Starting the server on port %d.\n", SERVER_PORT);
	SocketDescriptor sd(SERVER_PORT, 0);
	StartupResult result = m_peerInterface->Startup(MAX_CLIENTS, &m_socketDescriptor, 1);
	m_peerInterface->SetMaximumIncomingConnections(MAX_CLIENTS); // Max number of players.
	m_peerInterface->SetOccasionalPing(true);

	if (result != StartupResult::RAKNET_STARTED)
	{
		printf("Error: failure to start the RakNet peer interface.\n");
		return -1;
	}

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
	UpdateBall();
	
	BitStream bsOut;
	bsOut.Write((MessageID) GameMessages::UPDATE_TICK);
	bsOut.Write(m_ball.GetPosition());
	bsOut.Write(m_ball.GetVelocity());
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Slime* slime = it->second;

		if (slime->HasJoinedGame())
		{
			RakString name = slime->GetName().c_str();
			bsOut.Write(slime->GetPlayerId());
			bsOut.Write(slime->GetPosition());
			bsOut.Write(slime->GetVelocity());
		}
	}
	bsOut.Write((int) -1);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE,
		0, RakNet::UNASSIGNED_RAKNET_GUID, true);
	
	//GameMessages::UPDATE_TICK
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
			// SERVER SPECIFIC MESSAGES:
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		{
			printf("Our opponent has disconnected.\n");
			break;
		}
		case ID_REMOTE_CONNECTION_LOST:
		{
			printf("Our opponent has lost the connection.\n");
			break;
		}
		case ID_REMOTE_NEW_INCOMING_CONNECTION: // A player has connected.
		case ID_NEW_INCOMING_CONNECTION:
		{
			BitStream bsOut;
			bsOut.Write((MessageID)GameMessages::ACCEPT_CONNECTION);
			bsOut.Write(m_playerIdCounter);

			for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
			{
				Slime* slime = it->second;

				if (slime->HasJoinedGame())
				{
					RakString name = slime->GetName().c_str();
					bsOut.Write(slime->GetPlayerId());
					bsOut.Write(name);
					bsOut.Write(slime->GetTeamIndex());
					bsOut.Write(slime->GetColor());
				}
			}
			bsOut.Write((int)-1);
			m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
				RELIABLE_ORDERED, 0, packet->systemAddress, false);

			printf("Player ID %d has connected.\n", m_playerIdCounter);
			m_guidToPlayerMap[packet->guid] = m_playerIdCounter;
			m_players[m_playerIdCounter] = new Slime(Color::RED, m_config.SLIME_RADIUS, Vector2f::ZERO);
			m_playerIdCounter++;

			break;
		}

		// CLIENT SPECIFIC MESSAGES:
		case ID_NO_FREE_INCOMING_CONNECTIONS:
		{
			printf("That address is already playing a game!\n");
			break;
		}
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			printf("We have connected to our opponent!\n");
			//gameData->opponentAddress = packet->systemAddress;
			//gameData->isConnected = true;
			break;
		}

		// SERVER/CLIENT SHARED MESSAGES:
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("We have been disconnected.\n");
			//gameData->isConnected = false;
			break;
		}
		case ID_CONNECTION_LOST:
		{
			int playerId = m_guidToPlayerMap[packet->guid];
			Slime* player = m_players[playerId];
			printf("Player ID %d has diconnected.\n", playerId);
			m_players.erase(playerId);
			m_guidToPlayerMap.erase(packet->guid);
			break;
		}
		case GameMessages::DISCONNECTED:
		{
			int playerId = m_guidToPlayerMap[packet->guid];
			Slime* player = m_players[playerId];
			printf("Player ID %d has diconnected.\n", playerId);
			m_players.erase(playerId);
			m_guidToPlayerMap.erase(packet->guid);
			break;
		}
		case GameMessages::JOIN_GAME:
		{
			RakString name;
			Color color;
			int teamIndex;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(name);
			bsIn.Read(teamIndex);
			bsIn.Read(color);

			int playerId = m_guidToPlayerMap[packet->guid];
			Slime* player = m_players[playerId];
			player->SetName(name.C_String());
			player->SetTeamIndex(teamIndex);
			player->SetColor(color);
			player->SetJoinedGame(true);

			printf("%s (ID %d) has joined team %d!\n", name.C_String(), playerId, teamIndex);
			break;
		}
		case GameMessages::CLIENT_UPDATE_TICK:
		{
			Vector2f playerPos;
			Vector2f playerVel;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(playerPos);
			bsIn.Read(playerVel);

			int playerId = m_guidToPlayerMap[packet->guid];
			Slime* player = m_players[playerId];
			player->SetPosition(playerPos);
			player->SetVelocity(playerVel);
			break;
		}
		}
	}
}


void Server::UpdateBall()
{
	Vector2f position = m_ball.GetPosition();
	Vector2f velocity = m_ball.GetVelocity();

	velocity.y += m_config.BALL_GRAVITY;
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
				velocity.y -= m_config.BALL_GRAVITY * 0.5f;

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

	// Collide with the net.
	// TODO: Circle to Rectangle (line?) collision.

	// Collide with the floor and walls.
	if (position.y + m_ball.GetRadius() >= m_config.FLOOR_Y)
	{
		position.y = m_config.FLOOR_Y - m_ball.GetRadius();
		if (velocity.y > 0.0f)
			velocity.y = -velocity.y;
	}
	if (position.x - m_ball.GetRadius() <= 0.0f)
	{
		position.x = m_ball.GetRadius();
		if (velocity.x < 0.0f)
			velocity.x = -velocity.x;
	}
	else if (position.x + m_ball.GetRadius() >= m_config.VIEW_WIDTH)
	{
		position.x = m_config.VIEW_WIDTH - m_ball.GetRadius();
		if (velocity.x > 0.0f)
			velocity.x = -velocity.x;
	}

	// Limit max speed.
	float velLength = velocity.Length();
	if (velLength > m_config.BALL_MAX_SPEED)
		velocity *= m_config.BALL_MAX_SPEED / velLength;

	m_ball.SetPosition(position);
	m_ball.SetVelocity(velocity);
}

