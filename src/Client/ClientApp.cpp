#include "ClientApp.h"
#include <GameLib/graphics/Graphics.h>
#include <Common/Config.h>
#include <math/MathLib.h>

using namespace RakNet;

ClientApp::ClientApp(RakNet::RakPeerInterface* peerInterface) :
	m_peerInterface(peerInterface)
{
	m_player = new Slime(
		m_config.SLIME_COLORS[0],
		m_config.SLIME_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.25f, m_config.FLOOR_Y));
}

ClientApp::~ClientApp()
{
	// Delete all players.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		delete it->second;
	}

	m_players.clear();
	m_player = NULL;
}

void ClientApp::ReadConnectionAcceptedPacket(RakNet::Packet* packet)
{
	int playerId;

	m_serverGuid = packet->guid;

	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(playerId);
	m_player->SetPlayerId(playerId);

	while (true)
	{
		Color color;
		RakString name;
		int teamIndex;
		bsIn.Read(playerId);
		if (playerId < 0)
			break;
		bsIn.Read(name);
		bsIn.Read(teamIndex);
		bsIn.Read(color);

		Slime* slime = new Slime;
		slime->SetRadius(m_config.SLIME_RADIUS);
		slime->SetName(name.C_String());
		slime->SetTeamIndex(teamIndex);
		slime->SetColor(color);
		m_players[playerId] = slime;
	}

	printf("Connection Request Accepted. We are player ID %d.\n", m_player->GetPlayerId());

	m_players[m_player->GetPlayerId()] = m_player;
}

void ClientApp::OnInitialize()
{
	m_ball = Ball(m_config.BALL_COLOR, m_config.BALL_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.25f,
			m_config.FLOOR_Y - m_config.BALL_SERVE_HEIGHT));

	m_teams[0] = Team(0, "1", Rect2f(0.0f, 0.0f,
		(m_config.VIEW_WIDTH - m_config.NET_WIDTH) * 0.5f, m_config.FLOOR_Y));
	m_teams[1] = Team(1, "2", Rect2f((m_config.VIEW_WIDTH + m_config.NET_WIDTH) * 0.5f,
		0.0f, (m_config.VIEW_WIDTH - m_config.NET_WIDTH) * 0.5f, m_config.FLOOR_Y));

	m_state = STATE_CHOOSE_COLOR;

	m_selectedColorButtonIndex = -1;
	m_chooseColorButtonRadius = m_config.VIEW_HEIGHT * 0.05f;
	
	for (int i = 0; i < 2; i++)
	{
		Rect2f button(m_config.VIEW_WIDTH * (0.25f + (i * 0.5f)), m_config.VIEW_HEIGHT * 0.5f, 0.0f, 0.0f);
		button.Inflate(
			m_config.VIEW_WIDTH * 0.5f * 0.5f * 0.6f,
			m_config.VIEW_HEIGHT * 0.05f);
		m_joinTeamButtons.push_back(button);
	}
		
	for (int i = 0; i < 9; i++)
	{
		float x = ((i % 3) - 1) * m_config.VIEW_HEIGHT * 0.2f;
		float y = ((i / 3) - 1) * m_config.VIEW_HEIGHT * 0.2f;

		m_chooseColorButtons.push_back(Vector2f(
			(m_config.VIEW_WIDTH * 0.5f) + x,
			(m_config.VIEW_HEIGHT * 0.5f) + y));
	}
}

void ClientApp::OnUpdate(float timeDelta)
{
	// Ball interpolation.
	if (m_state == STATE_PLAY_GAME)
	{
		m_ball.SetPosition(m_ball.GetPosition() + m_ball.GetVelocity());
		m_ball.SetVelocity(m_ball.GetVelocity() + Vector2f(0.0f, m_config.BALL_GRAVITY));
	}

	ReceivePackets();

	if (GetKeyboard()->IsKeyPressed(Keys::ESCAPE))
	{
		BitStream bsOut;
		bsOut.Write((MessageID) GameMessages::DISCONNECTED);
		m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
			RELIABLE_ORDERED, 0, m_serverGuid, false);
		Quit();
		return;
	}

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	if (m_state == STATE_CHOOSE_COLOR)
	{
		m_selectedColorButtonIndex = -1;

		// Check for hovering over color option buttons.
		for (int i = 0; i < m_config.NUM_SLIME_COLORS; i++)
		{
			if (mousePos.DistTo(m_chooseColorButtons[i]) < m_chooseColorButtonRadius * 1.2f)
			{
				m_selectedColorButtonIndex = i;
				m_player->SetColor(m_config.SLIME_COLORS[i]);
			}
		}

		// Check for clicking on a color option button.
		if (m_selectedColorButtonIndex >= 0 &&
			GetMouse()->IsButtonPressed(MouseButtons::LEFT))
		{
			m_state = STATE_CHOOSE_TEAM;
		}

		m_player->SetPosition(Vector2f(m_config.VIEW_WIDTH * 0.5f, m_config.VIEW_HEIGHT * 0.9f));
	}
	else if (m_state == STATE_CHOOSE_TEAM)
	{
		m_selectedColorButtonIndex = -1;

		// Check for hovering over join team buttons.
		for (int i = 0; i < 2; i++)
		{
			if (m_joinTeamButtons[i].Contains(mousePos))
			{
				m_selectedColorButtonIndex = i;
				m_player->SetTeamIndex(1 - i);
			}
		}

		// Check for clicking on a join team button.
		if (m_selectedColorButtonIndex >= 0 &&
			GetMouse()->IsButtonPressed(MouseButtons::LEFT))
		{
			m_state = STATE_PLAY_GAME;
			m_player->SetTeamIndex(m_selectedColorButtonIndex);
			m_player->SetPosition(Vector2f(
				m_config.VIEW_WIDTH * (0.25f + (0.5f * m_player->GetTeamIndex())),
				m_config.FLOOR_Y));

			BitStream bsOut;
			bsOut.Write((MessageID) GameMessages::JOIN_GAME);
			bsOut.Write(m_player->GetName().c_str());
			bsOut.Write(m_player->GetTeamIndex());
			bsOut.Write(m_player->GetColor());
			m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
				RELIABLE_ORDERED, 0, m_serverGuid, false);
			printf("Joining team %d\n", m_player->GetTeamIndex());
		}
		else
		{
			m_player->SetPosition(Vector2f(m_config.VIEW_WIDTH * 0.5f, m_config.VIEW_HEIGHT * 0.9f));
		}
	}
	else if (m_state == STATE_PLAY_GAME)
	{
		UpdatePlayer();

		BitStream bsOut;
		bsOut.Write((MessageID) GameMessages::CLIENT_UPDATE_TICK);
		bsOut.Write(m_player->GetPosition());
		bsOut.Write(m_player->GetVelocity());
		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE,
			0, m_serverGuid, false);
	}
	else if (m_state == STATE_WAIT_FOR_SERVE)
	{

	}
}

void ClientApp::UpdatePlayer()
{
	Vector2f pos = m_player->GetPosition();
	Vector2f vel = m_player->GetVelocity();
	Team* team = &m_teams[m_player->GetTeamIndex()];
	float minX = team->GetPlayRegion().GetLeft();
	float maxX = team->GetPlayRegion().GetRight();

	// Integrate gravitational force.
	vel.y += m_config.SLIME_GRAVITY;

	// Update controls.
	if (GetKeyboard()->IsKeyDown(Keys::LEFT))
		pos.x -= 4.5f;
	if (GetKeyboard()->IsKeyDown(Keys::RIGHT))
		pos.x += 4.5f;
	if (GetKeyboard()->IsKeyDown(Keys::UP) && pos.y >= m_config.FLOOR_Y)
		vel.y = -m_config.SLIME_JUMP_SPEED;

	// Collide with floor and walls.
	if (pos.y >= m_config.FLOOR_Y)
	{
		pos.y = m_config.FLOOR_Y;
		if (vel.y > 0.0f)
			vel.y = 0.0f;
	}
	if (pos.x - m_player->GetRadius() <= minX)
	{
		pos.x = minX + m_player->GetRadius();
		vel.x = 0.0f;
	}
	else if (pos.x + m_player->GetRadius() >= maxX)
	{
		pos.x = maxX - m_player->GetRadius();
		vel.x = 0.0f;
	}

	// Integrate velocity.
	pos += vel;

	m_player->SetPosition(pos);
	m_player->SetVelocity(vel);
}

void ClientApp::ReceivePackets()
{
	Packet *packet;

	for (packet = m_peerInterface->Receive(); packet != NULL;
		m_peerInterface->DeallocatePacket(packet),
		packet = m_peerInterface->Receive())
	{
		switch (packet->data[0])
		{
		// SERVER/CLIENT SHARED MESSAGES:
		case ID_DISCONNECTION_NOTIFICATION:
		{
			printf("We have been disconnected.\n");
			break;
		}
		case ID_CONNECTION_LOST:
		{
			printf("Connection lost.\n");
			break;
		}
		case GameMessages::PLAYER_JOINED:
		{
			int playerId;
			RakString name;
			Color color;
			int teamIndex;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(playerId);
			bsIn.Read(name);
			bsIn.Read(teamIndex);
			bsIn.Read(color);

			if (playerId != m_player->GetPlayerId())
			{
				Slime* player;
				if (m_players.find(playerId) != m_players.end())
				{
					player = m_players[playerId];
				}
				else
				{
					player = new Slime();
					m_players[playerId] = player;
				}
				player->SetRadius(m_config.SLIME_RADIUS);
				player->SetName(name.C_String());
				player->SetTeamIndex(teamIndex);
				player->SetColor(color);
				player->SetJoinedGame(true);
				printf("%s (ID %d) joined team %d!\n", name.C_String(), playerId, teamIndex);
			}
			break;
		}
		case GameMessages::PLAYER_LEFT:
		{
			int playerId;
			RakString name;
			Color color;
			int teamIndex;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(playerId);
			bsIn.Read(name);
			bsIn.Read(teamIndex);
			bsIn.Read(color);

			if (playerId != m_player->GetPlayerId())
			{
				Slime* player = m_players[playerId];
				printf("%s (ID %d) left the game.\n", player->GetName().c_str(), playerId);
				delete player;
				m_players.erase(playerId);
			}
			break;
		}
		case GameMessages::TEAM_SCORED:
		{
			int scoringTeamIndex;
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(scoringTeamIndex);
			m_teams[scoringTeamIndex].SetScore(m_teams[scoringTeamIndex].GetScore() + 1);
			printf("Team %d scored! [Team 0: %d | Team 1: %d]\n",
				scoringTeamIndex, m_teams[0].GetScore(), m_teams[1].GetScore());
			m_state = STATE_WAIT_FOR_SERVE;
			break;
		}
		case GameMessages::TEAM_SERVE:
		{
			if (m_state != STATE_CHOOSE_COLOR && m_state != STATE_CHOOSE_TEAM)
			{
				m_player->SetPosition(Vector2f(
					m_config.VIEW_WIDTH * (0.25f + (0.5f * m_player->GetTeamIndex())),
					m_config.FLOOR_Y));
				m_player->SetVelocity(Vector2f::ZERO);
				m_state = STATE_PLAY_GAME;
			}
			break;
		}
		case GameMessages::UPDATE_TICK:
		{
			Vector2f ballPos;
			Vector2f ballVel;
			
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(ballPos);
			bsIn.Read(ballVel);
			m_ball.SetPosition(ballPos);
			m_ball.SetVelocity(ballVel);

			while (true)
			{
				int playerId;
				Color color;
				RakString name;
				bsIn.Read(playerId);
				if (playerId < 0)
					break;
				bsIn.Read(ballPos);
				bsIn.Read(ballVel);
				if (playerId != m_player->GetPlayerId())
				{
					if (m_players.find(playerId) != m_players.end())
					{
						m_players[playerId]->SetPosition(ballPos);
						m_players[playerId]->SetVelocity(ballVel);
					}
				}
			}
			break;
		}
		}
	}
}

void ClientApp::DrawSlime(Graphics& g, const Slime& slime, const Vector2f& lookAtPoint)
{
	Polygonf slimePoly;
	int numEdges = 10;
	for (int i = 0; i <= numEdges; i++)
	{
		float angle = (i / (float)numEdges) * Math::PI;
		slimePoly.AddVertex(Vector2f(Math::Cos(angle), -Math::Sin(angle)));
	}

	// Calculate eye position and scales.
	float eyeRadius = 1.0f / 6.0f;
	float pupilRadius = 0.5f;
	float sinQuarterPi = sin(Math::PI * 0.25f);
	Vector2f eyeOffset = Vector2f(sinQuarterPi, -sinQuarterPi) * (1.0f - eyeRadius);
	Vector2f pupilOffset = Vector2f::Normalize(lookAtPoint - slime.GetPosition()) * (1.0f - pupilRadius);
	if (slime.GetTeamIndex() == 1)
		eyeOffset.x = -eyeOffset.x;

	// Draw the body.
	g.Translate(slime.GetPosition());
	g.Scale(slime.GetRadius());
	g.FillPolygon(slimePoly, slime.GetColor());

	// Draw the eye white.
	g.Translate(eyeOffset);
	g.Scale(eyeRadius);
	g.FillCircle(Vector2f::ZERO, 1.0f, Color::WHITE);

	// Draw the pupil.
	g.Translate(pupilOffset);
	g.Scale(pupilRadius);
	g.FillCircle(Vector2f::ZERO, 1.0f, Color::BLACK);

	g.ResetTransform();
}

void ClientApp::OnRender()
{
	Rect2f groundRect(0, m_config.FLOOR_Y, m_config.VIEW_WIDTH,
		m_config.VIEW_HEIGHT - m_config.FLOOR_Y);
	Rect2f netRect((m_config.VIEW_WIDTH - m_config.NET_WIDTH) / 2,
		m_config.FLOOR_Y - m_config.NET_HEIGHT + m_config.NET_DEPTH,
		m_config.NET_WIDTH, m_config.NET_HEIGHT);

	Vector2f mousePos((float)GetMouse()->GetX(), (float)GetMouse()->GetY());

	// Set up a graphics drawer.
	Graphics g(GetWindow());
	g.SetViewport(Viewport(0, 0, GetWindow()->GetWidth(),
		GetWindow()->GetHeight()), false, true);
	g.SetProjection(Matrix4f::CreateOrthographic(0.0f,
		m_config.VIEW_WIDTH, m_config.VIEW_HEIGHT, 0.0f, 1.0f, -1.0f));
	g.ResetTransform();

	// Clear the sky color.
	g.Clear(m_config.SKY_COLOR);

	// Draw the ground.
	g.FillRect(groundRect, m_config.GROUND_COLOR);

	// Draw the net.
	g.FillRect(netRect, m_config.NET_COLOR);

	// Draw the ball.
	g.FillCircle(m_ball.GetPosition(), m_ball.GetRadius(), m_ball.GetColor());

	// Draw the slimes.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		Slime* slime = it->second;
		Vector2f lookAtPoint = m_ball.GetPosition();
		if (slime == m_player && (m_state == STATE_CHOOSE_COLOR || m_state == STATE_CHOOSE_TEAM))
			lookAtPoint = mousePos;
		DrawSlime(g, *slime, lookAtPoint);
	}

	// Draw state-specific stuff.
	if (m_state == STATE_CHOOSE_COLOR)
	{
		// Draw color options.
		for (int i = 0; i < m_config.NUM_SLIME_COLORS; i++)
		{
			float r = m_chooseColorButtonRadius;
			if (m_selectedColorButtonIndex == i)
				r *= 1.2f;
			g.FillCircle(m_chooseColorButtons[i], r, m_config.SLIME_COLORS[i]);
			g.DrawCircle(m_chooseColorButtons[i], r, Color::BLACK);
		}
	}
	else if (m_state == STATE_CHOOSE_TEAM)
	{
		// Draw team buttons.
		for (int i = 0; i < 2; i++)
		{
			g.FillRect(m_joinTeamButtons[i], Color::WHITE);
		}
	}
	else if (m_state == STATE_PLAY_GAME || m_state == STATE_WAIT_FOR_SERVE)
	{
		// Draw score.
	}
}