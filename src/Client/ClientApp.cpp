#include "ClientApp.h"
#include <GameLib/graphics/Graphics.h>
#include <Common/Config.h>
#include <math/MathLib.h>
#include <sstream>

using namespace RakNet;

ClientApp::ClientApp(RakNet::RakPeerInterface* peerInterface) :
	m_peerInterface(peerInterface),
	m_fontSmall(NULL),
	m_fontScore(NULL),
	m_player(NULL)
{
	// Default color scheme:
	//m_colorScheme.ballColor							= Color::YELLOW;
	//m_colorScheme.groundColor						= Color::GRAY;
	//m_colorScheme.netColor							= Color::WHITE;
	//m_colorScheme.skyColor							= Color::BLUE;
	//m_colorScheme.slime.eyeColor					= Color::WHITE;
	//m_colorScheme.slime.pupilColor					= Color::BLACK;
	//m_colorScheme.ui.chooseColorButton.outlineColor	= Color::BLACK;
	//m_colorScheme.ui.joinTeamButton.backgroundColor	= Color::WHITE;
	//m_colorScheme.ui.joinTeamButton.textColor		= Color::BLUE;
	//m_colorScheme.ui.gameInfoTextColor				= Color::YELLOW;
	//m_colorScheme.ui.menuPromptTextColor			= Color::YELLOW;
	//m_colorScheme.ui.scoreTextColor					= Color::YELLOW;
	//m_colorScheme.slime.bodyColors.clear();
	//m_colorScheme.slime.bodyColors.push_back(Color::RED);
	//m_colorScheme.slime.bodyColors.push_back(Color(255, 128, 0));
	//m_colorScheme.slime.bodyColors.push_back(Color::YELLOW);
	//m_colorScheme.slime.bodyColors.push_back(Color::GREEN);
	//m_colorScheme.slime.bodyColors.push_back(Color::CYAN);
	//m_colorScheme.slime.bodyColors.push_back(Color(10, 10, 255));
	//m_colorScheme.slime.bodyColors.push_back(Color::MAGENTA);
	//m_colorScheme.slime.bodyColors.push_back(Color(210, 220, 220));
	//m_colorScheme.slime.bodyColors.push_back(Color::BLACK);

	// Halloween color scheme:
	//m_colorScheme.ballColor							= Color(226, 98, 0);
	//m_colorScheme.groundColor						= Color(52, 43, 56);
	//m_colorScheme.netColor							= Color::WHITE;
	//m_colorScheme.skyColor							= Color(3, 0, 11);
	//m_colorScheme.slime.eyeColor					= Color::WHITE;
	//m_colorScheme.slime.pupilColor					= Color::BLACK;
	//m_colorScheme.ui.chooseColorButton.outlineColor	= Color::WHITE;
	//m_colorScheme.ui.joinTeamButton.backgroundColor	= Color(170, 96, 201);
	//m_colorScheme.ui.joinTeamButton.textColor		= m_colorScheme.skyColor;
	//m_colorScheme.ui.gameInfoTextColor				= Color::GREEN;
	//m_colorScheme.ui.menuPromptTextColor			= Color::GREEN;
	//m_colorScheme.ui.scoreTextColor					= Color::GREEN;
	//m_colorScheme.slime.bodyColors.clear();
	//m_colorScheme.slime.bodyColors.push_back(Color(214, 0, 0));
	//m_colorScheme.slime.bodyColors.push_back(Color(226, 98, 0));
	//m_colorScheme.slime.bodyColors.push_back(Color(255, 241, 45));
	//m_colorScheme.slime.bodyColors.push_back(Color(134, 224, 17));
	//m_colorScheme.slime.bodyColors.push_back(Color(19, 219, 219));
	//m_colorScheme.slime.bodyColors.push_back(Color(0, 0, 242));
	//m_colorScheme.slime.bodyColors.push_back(Color(117, 8, 117));
	//m_colorScheme.slime.bodyColors.push_back(Color(210, 220, 220));
	//m_colorScheme.slime.bodyColors.push_back(Color::BLACK);

	// Thanksgiving color theme:
	m_colorScheme.ballColor							= Color(138, 160, 64);
	m_colorScheme.groundColor						= Color(183, 130, 66);
	m_colorScheme.netColor							= Color(242, 181, 26);
	m_colorScheme.skyColor							= Color(204, 192, 177);
	m_colorScheme.slime.eyeColor					= Color::WHITE;
	m_colorScheme.slime.pupilColor					= Color::BLACK;
	m_colorScheme.ui.chooseColorButton.outlineColor	= Color::BLACK;
	m_colorScheme.ui.joinTeamButton.backgroundColor	= Color(196, 89, 27);
	m_colorScheme.ui.joinTeamButton.textColor		= m_colorScheme.skyColor;
	m_colorScheme.ui.gameInfoTextColor				= Color::BLACK;
	m_colorScheme.ui.menuPromptTextColor			= Color::BLACK;
	m_colorScheme.ui.scoreTextColor					= Color::BLACK;
	m_colorScheme.slime.bodyColors.clear();
	m_colorScheme.slime.bodyColors.push_back(Color(249, 83, 27));
	m_colorScheme.slime.bodyColors.push_back(Color(196, 89, 27));
	m_colorScheme.slime.bodyColors.push_back(Color(242, 181, 26));
	m_colorScheme.slime.bodyColors.push_back(Color(138, 160, 64));
	m_colorScheme.slime.bodyColors.push_back(Color(95, 244, 202));
	m_colorScheme.slime.bodyColors.push_back(Color(19, 151, 221));
	m_colorScheme.slime.bodyColors.push_back(Color(145, 43, 102));
	m_colorScheme.slime.bodyColors.push_back(Color(206, 192, 163));
	m_colorScheme.slime.bodyColors.push_back(Color(76, 71, 56));

	// TODO: Christmas color theme:
}

ClientApp::~ClientApp()
{
	delete m_fontScore;
	delete m_fontSmall;

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
	int playerId, score1, score2;

	m_serverGuid = packet->guid;

	m_networkManager.Initialize(m_peerInterface, &m_inputManager, packet->systemAddress);

	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(playerId);
	bsIn.Read(m_gameConfig);
	bsIn.Read(score1);
	bsIn.Read(score2);

	m_player = new Slime(playerId, 0, 0, m_gameConfig.slime.radius);
	m_players[playerId] = m_player;

	m_teams[0].SetScore(score1);
	m_teams[1].SetScore(score2);

	while (true)
	{
		int colorIndex;
		int teamIndex;
		bsIn.Read(playerId);
		if (playerId < 0)
			break;
		bsIn.Read(teamIndex);
		bsIn.Read(colorIndex);

		Slime* slime = new Slime(playerId, teamIndex, colorIndex, m_gameConfig.slime.radius);
		m_players[playerId] = slime;
	}

	printf("Connection Request Accepted. We are player ID %d.\n", m_player->GetPlayerId());

	m_players[m_player->GetPlayerId()] = m_player;
}

void ClientApp::OnInitialize()
{
	m_inputManager.Initialize(GetKeyboard());

	// Load resources.
	//m_fontScore = Font::LoadFont("../../assets/AlteHaasGroteskBold.ttf", 24, 32, 128);
	//m_fontSmall = Font::LoadFont("../../assets/AlteHaasGroteskRegular.ttf", 12, 32, 128);
	m_fontScore = Font::LoadFont("assets/AlteHaasGroteskBold.ttf", 24, 32, 128);
	m_fontSmall = Font::LoadFont("assets/AlteHaasGroteskRegular.ttf", 12, 32, 128);

	// Create the two teams.
	m_teams[0] = Team(0, "1", Rect2f(0.0f, 0.0f,
		(m_gameConfig.view.width - m_gameConfig.net.width) * 0.5f, m_gameConfig.view.floorY), m_gameConfig);
	m_teams[1] = Team(1, "2", Rect2f((m_gameConfig.view.width + m_gameConfig.net.width) * 0.5f,
		0.0f, (m_gameConfig.view.width - m_gameConfig.net.width) * 0.5f, m_gameConfig.view.floorY), m_gameConfig);

	m_ball = Ball(m_gameConfig.ball.radius, Vector2f(m_gameConfig.view.width * 0.25f,
			m_gameConfig.view.floorY - m_gameConfig.ball.serveHeight));

	m_selectedColorButtonIndex = -1;
	m_chooseColorButtonRadius = m_gameConfig.view.height * 0.05f;

	for (int i = 0; i < 2; i++)
	{
		Rect2f button(
			m_gameConfig.view.width * (0.25f + (i * 0.5f)),
			m_gameConfig.view.height * 0.5f, 0.0f, 0.0f);
		button.Inflate(
			m_gameConfig.view.width * 0.5f * 0.5f * 0.6f,
			m_gameConfig.view.height * 0.05f);
		m_joinTeamButtons.push_back(button);
	}

	for (int i = 0; i < 9; i++)
	{
		float x = ((i % 3) - 1) * m_gameConfig.view.height * 0.2f;
		float y = ((i / 3) - 1) * m_gameConfig.view.height * 0.2f;

		m_chooseColorButtons.push_back(Vector2f(
			(m_gameConfig.view.width * 0.5f) + x,
			(m_gameConfig.view.height * 0.5f) + y));
	}

	m_state = STATE_CHOOSE_COLOR;
}

void ClientApp::OnUpdate(float timeDelta)
{
	ReceivePackets();

	// Escape: Quit the game.
	if (GetKeyboard()->IsKeyPressed(Keys::ESCAPE))
	{
		m_peerInterface->CloseConnection(m_serverGuid, true, 0, IMMEDIATE_PRIORITY);
		Quit();
		return;
	}

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	if (m_state == STATE_CHOOSE_COLOR)
	{
		m_selectedColorButtonIndex = -1;

		// Check for hovering over color option buttons.
		for (int i = 0; i < (int) m_colorScheme.slime.bodyColors.size(); i++)
		{
			if (mousePos.DistTo(m_chooseColorButtons[i]) < m_chooseColorButtonRadius * 1.2f)
			{
				m_selectedColorButtonIndex = i;
				m_player->SetColorIndex(i);
			}
		}

		// Check for clicking on a color option button.
		if (m_selectedColorButtonIndex >= 0 &&
			GetMouse()->IsButtonPressed(MouseButtons::LEFT))
		{
			m_state = STATE_CHOOSE_TEAM;
		}

		m_player->SetPosition(Vector2f(m_gameConfig.view.width * 0.5f, m_gameConfig.view.height * 0.9f));
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
				m_gameConfig.view.width * (0.25f + (0.5f * m_player->GetTeamIndex())),
				m_gameConfig.view.floorY));

			BitStream bsOut;
			bsOut.Write((MessageID) PacketType::JOIN_TEAM);
			bsOut.Write(m_player->GetTeamIndex());
			bsOut.Write(m_player->GetColorIndex());
			m_peerInterface->Send(&bsOut, HIGH_PRIORITY,
				RELIABLE_ORDERED, 0, m_serverGuid, false);
			printf("Joining team %d\n", m_player->GetTeamIndex());
		}
		else
		{
			m_player->SetPosition(Vector2f(m_gameConfig.view.width * 0.5f, m_gameConfig.view.height * 0.9f));
		}
	}
	else if (m_state == STATE_PLAY_GAME)
	{
		//UpdatePlayer();

		/*BitStream bsOut;
		bsOut.Write((MessageID) PacketType::CLIENT_UPDATE_TICK);
		m_player->SerializeDynamics(bsOut, m_gameConfig);
		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE,
			0, m_serverGuid, false);*/

		m_inputManager.Update(timeDelta);
		if (m_inputManager.GetMoveList().GetMoveCount() >= 1)
		{
			m_networkManager.SendInputPacket();
		}
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
	vel.y += m_gameConfig.slime.gravity;

	// Update controls.
	if (GetKeyboard()->IsKeyDown(Keys::LEFT))
		pos.x -= 4.5f;
	if (GetKeyboard()->IsKeyDown(Keys::RIGHT))
		pos.x += 4.5f;
	if (GetKeyboard()->IsKeyDown(Keys::UP) && pos.y >= m_gameConfig.view.floorY)
		vel.y = -m_gameConfig.slime.jumpSpeed;

	// Collide with floor and walls.
	if (pos.y >= m_gameConfig.view.floorY)
	{
		pos.y = m_gameConfig.view.floorY;
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
		case PacketType::PLAYER_JOINED:
		{
			int playerId;
			int colorIndex;
			int teamIndex;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(playerId);
			bsIn.Read(teamIndex);
			bsIn.Read(colorIndex);

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
				player->SetRadius(m_gameConfig.slime.radius);
				player->SetTeamIndex(teamIndex);
				player->SetColorIndex(colorIndex);
				player->SetJoinedGame(true);
				printf("Player ID %d joined team %d!\n",  playerId, teamIndex);
			}
			break;
		}
		case PacketType::PLAYER_LEFT:
		{
			int playerId;
			Color color;
			int teamIndex;

			// Read the packet.
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(playerId);
			bsIn.Read(teamIndex);
			bsIn.Read(color);

			if (playerId != m_player->GetPlayerId() && m_players.find(playerId) != m_players.end())
			{
				Slime* player = m_players[playerId];
				printf("Player ID %d left the game.\n", playerId);
				delete player;
				m_players.erase(playerId);
			}
			break;
		}
		case PacketType::TEAM_SCORED:
		{
			int scoringTeamIndex;
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));
			bsIn.Read(scoringTeamIndex);
			m_teams[scoringTeamIndex].SetScore(m_teams[scoringTeamIndex].GetScore() + 1);
			printf("Team %d scored!\n", scoringTeamIndex);
			m_state = STATE_WAIT_FOR_SERVE;
			break;
		}
		case PacketType::TEAM_SERVE:
		{
			if (m_state != STATE_CHOOSE_COLOR && m_state != STATE_CHOOSE_TEAM)
			{
				m_player->SetPosition(m_teams[m_player->GetTeamIndex()]
					.GetPlayerSpawnPosition());
				m_player->SetVelocity(Vector2f::ZERO);
				m_state = STATE_PLAY_GAME;
			}
			break;
		}
		case PacketType::UPDATE_TICK:
		{
			Vector2f ballPos;
			Vector2f ballVel;
			
			BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(MessageID));

			// Read ball information.
			bsIn.Read(ballPos);
			bsIn.Read(ballVel);
			m_ball.SetPosition(ballPos);
			m_ball.SetVelocity(ballVel);

			// Read player information.
			Slime dummy;
			int playerId;

			while (true)
			{
				bsIn.Read(playerId);
				if (playerId < 0)
					break;

				if (/*playerId != m_player->GetPlayerId() && */m_players.find(playerId) != m_players.end())
				{
					m_players[playerId]->DeserializeDynamics(bsIn, m_gameConfig);
				}
				else
				{
					// We still need to deserialize the data.
					dummy.DeserializeDynamics(bsIn, m_gameConfig);
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
	int numEdges = 16;
	for (int i = 0; i <= numEdges; i++)
	{
		float angle = (i / (float) numEdges) * Math::PI;
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
	g.FillPolygon(slimePoly, m_colorScheme.slime.bodyColors[slime.GetColorIndex()]);

	// Draw the eye white.
	g.Translate(eyeOffset);
	g.Scale(eyeRadius);
	g.FillCircle(Vector2f::ZERO, 1.0f, m_colorScheme.slime.eyeColor);

	// Draw the pupil.
	g.Translate(pupilOffset);
	g.Scale(pupilRadius);
	g.FillCircle(Vector2f::ZERO, 1.0f, m_colorScheme.slime.pupilColor);

	g.ResetTransform();
}

void ClientApp::OnRender()
{
	Rect2f groundRect(0, m_gameConfig.view.floorY, m_gameConfig.view.width,
		m_gameConfig.view.height - m_gameConfig.view.floorY);
	Rect2f netRect((m_gameConfig.view.width - m_gameConfig.net.width) / 2,
		m_gameConfig.view.floorY  - m_gameConfig.net.height + m_gameConfig.net.depthBelowGround,
		m_gameConfig.net.width, m_gameConfig.net.height);

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	// Set up a graphics drawer.
	Graphics g(GetWindow());
	g.SetViewport(Viewport(0, 0, GetWindow()->GetWidth(),
		GetWindow()->GetHeight()), false, true);
	g.SetProjection(Matrix4f::CreateOrthographic(0.0f,
		m_gameConfig.view.width, m_gameConfig.view.height, 0.0f, 1.0f, -1.0f));
	g.ResetTransform();

	// Clear the sky color.
	g.Clear(m_colorScheme.skyColor);

	// Draw the ground.
	g.FillRect(groundRect, m_colorScheme.groundColor);

	// Draw the net.
	g.FillRect(netRect, m_colorScheme.netColor);

	// Draw the ball.
	g.FillCircle(m_ball.GetPosition(), m_ball.GetRadius(), m_colorScheme.ballColor);

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
		// Draw the prompt.
		g.DrawString(m_fontScore, "Choose a color!",
			m_gameConfig.view.width * 0.5f, m_gameConfig.view.height * 0.1f,
			m_colorScheme.ui.menuPromptTextColor, Align::TOP_CENTER);

		// Draw color options.
		for (int i = 0; i < (int) m_colorScheme.slime.bodyColors.size(); i++)
		{
			float r = m_chooseColorButtonRadius;
			if (m_selectedColorButtonIndex == i)
				r *= 1.2f;
			g.FillCircle(m_chooseColorButtons[i], r, m_colorScheme.slime.bodyColors[i]);
			g.DrawCircle(m_chooseColorButtons[i], r, m_colorScheme.ui.chooseColorButton.outlineColor);
		}
	}
	else if (m_state == STATE_CHOOSE_TEAM)
	{
		// Draw the prompt.
		g.DrawString(m_fontScore, "Choose a team!",
			m_gameConfig.view.width * 0.5f, m_gameConfig.view.height * 0.1f,
			m_colorScheme.ui.menuPromptTextColor, Align::TOP_CENTER);

		// Draw team buttons.
		for (int i = 0; i < 2; i++)
		{
			g.FillRect(m_joinTeamButtons[i], m_colorScheme.ui.joinTeamButton.backgroundColor);
			g.DrawString(m_fontSmall, "Click here to join this team!",
				m_joinTeamButtons[i].GetCenter().x,
				m_joinTeamButtons[i].GetCenter().y,
				m_colorScheme.ui.joinTeamButton.textColor, Align::CENTERED);
		}
	}
	else if (m_state == STATE_PLAY_GAME || m_state == STATE_WAIT_FOR_SERVE)
	{
		// Draw the score.
		std::stringstream strStream;
		strStream << m_teams[0].GetScore() << " - " << m_teams[1].GetScore();
		g.DrawString(m_fontScore, strStream.str(),
			m_gameConfig.view.width * 0.5f, m_gameConfig.view.height * 0.1f,
			m_colorScheme.ui.scoreTextColor, Align::TOP_CENTER);

		// Count the number of players per team.
		int playerCounts[2] = { 0, 0 };
		for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
		{
			playerCounts[it->second->GetTeamIndex()]++;
		}
		
		// Display if we're waiting for players.
		for (int i = 0; i < 2; i++)
		{
			if (playerCounts[i] == 0)
			{
				g.DrawString(m_fontSmall, "Waiting for players...",
					m_joinTeamButtons[i].GetCenter().x,
					m_joinTeamButtons[i].GetCenter().y,
					m_colorScheme.ui.gameInfoTextColor, Align::CENTERED);
			}
		}
	}
}