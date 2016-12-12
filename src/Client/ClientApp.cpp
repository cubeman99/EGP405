#include "ClientApp.h"
#include <GameLib/graphics/Graphics.h>
#include <GameLib/util/Timing.h>
#include <math/MathLib.h>
#include <sstream>
#include "PlayerState.h"

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
	//m_colorScheme.ballColor							= Color(138, 160, 64);
	//m_colorScheme.groundColor						= Color(183, 130, 66);
	//m_colorScheme.netColor							= Color(242, 181, 26);
	//m_colorScheme.skyColor							= Color(204, 192, 177);
	//m_colorScheme.slime.eyeColor					= Color::WHITE;
	//m_colorScheme.slime.pupilColor					= Color::BLACK;
	//m_colorScheme.ui.chooseColorButton.outlineColor	= Color::BLACK;
	//m_colorScheme.ui.joinTeamButton.backgroundColor	= Color(196, 89, 27);
	//m_colorScheme.ui.joinTeamButton.textColor		= m_colorScheme.skyColor;
	//m_colorScheme.ui.gameInfoTextColor				= Color::BLACK;
	//m_colorScheme.ui.menuPromptTextColor			= Color::BLACK;
	//m_colorScheme.ui.scoreTextColor					= Color::BLACK;
	//m_colorScheme.slime.bodyColors.clear();
	//m_colorScheme.slime.bodyColors.push_back(Color(249, 83, 27));
	//m_colorScheme.slime.bodyColors.push_back(Color(196, 89, 27));
	//m_colorScheme.slime.bodyColors.push_back(Color(242, 181, 26));
	//m_colorScheme.slime.bodyColors.push_back(Color(138, 160, 64));
	//m_colorScheme.slime.bodyColors.push_back(Color(95, 244, 202));
	//m_colorScheme.slime.bodyColors.push_back(Color(19, 151, 221));
	//m_colorScheme.slime.bodyColors.push_back(Color(145, 43, 102));
	//m_colorScheme.slime.bodyColors.push_back(Color(206, 192, 163));
	//m_colorScheme.slime.bodyColors.push_back(Color(76, 71, 56));

	// Christmas color theme:
	m_colorScheme.ballColor							= Color::RED;
	m_colorScheme.groundColor						= Color::WHITE;
	m_colorScheme.netColor							= Color(21, 119, 2);
	m_colorScheme.skyColor							= Color(96, 202, 255);
	m_colorScheme.slime.eyeColor					= Color::WHITE;
	m_colorScheme.slime.pupilColor					= Color::BLACK;
	m_colorScheme.ui.chooseColorButton.outlineColor	= Color::BLACK;
	m_colorScheme.ui.joinTeamButton.backgroundColor	= Color(21, 119, 2);
	m_colorScheme.ui.joinTeamButton.textColor		= Color::WHITE;
	m_colorScheme.ui.gameInfoTextColor				= Color::RED;
	m_colorScheme.ui.menuPromptTextColor			= Color::RED;
	m_colorScheme.ui.scoreTextColor					= Color(21, 119, 2);
	m_colorScheme.slime.bodyColors.clear();
	m_colorScheme.slime.bodyColors.push_back(Color::RED);
	m_colorScheme.slime.bodyColors.push_back(Color(255, 141, 2)); // orange
	m_colorScheme.slime.bodyColors.push_back(Color(255, 230, 45)); // gold
	m_colorScheme.slime.bodyColors.push_back(Color(103, 247, 42)); // lime  green
	m_colorScheme.slime.bodyColors.push_back(Color(12, 160, 1)); // dark green
	m_colorScheme.slime.bodyColors.push_back(Color(30, 92, 237)); // blue
	m_colorScheme.slime.bodyColors.push_back(Color(168, 1, 193)); // purple
	m_colorScheme.slime.bodyColors.push_back(Color(249, 251, 252)); // white
	m_colorScheme.slime.bodyColors.push_back(Color::BLACK);
}

ClientApp::~ClientApp()
{
	delete m_fontScore;
	delete m_fontSmall;
	m_player = NULL;
}

void ClientApp::ReadConnectionAcceptedPacket(RakNet::Packet* packet)
{
	int playerId, score1, score2;
	int colorIndex, teamIndex;
	GameConfig config;

	m_networkManager.Initialize(m_peerInterface, &m_inputManager, packet->systemAddress);
	
	BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(MessageID));
	bsIn.Read(playerId);
	bsIn.Read(config);
	bsIn.Read(score1);
	bsIn.Read(score2);

	m_gameWorld.Initialize(config);
	m_gameWorld.GetTeam(0).SetScore(score1);
	m_gameWorld.GetTeam(1).SetScore(score2);
	m_player = m_gameWorld.CreatePlayer(playerId);

	while (true)
	{
		bsIn.Read(playerId);
		if (playerId < 0)
			break;
		bsIn.Read(teamIndex);
		bsIn.Read(colorIndex);

		Slime* slime = m_gameWorld.CreatePlayer(playerId);
		slime->SetTeamIndex(teamIndex);
		slime->SetColorIndex(colorIndex);
	}

	printf("Connection Request Accepted. We are player ID %d.\n", m_player->GetPlayerId());
}

float ClientApp::GetTimeStamp()
{
	// TODO: fix this.
	return (float) Time::GetTime();
}

void ClientApp::OnInitialize()
{
	m_inputManager.Initialize(GetKeyboard());

	m_numMovesPerInputPacket = 3;
	m_enableClientSidePrediction = true;
	m_enableServerReconciliation = true;
	m_enableEntityInterpolation = true;

	// Load resources.
	m_fontScore = Font::LoadFont("assets/AlteHaasGroteskBold.ttf", 24, 32, 128);
	m_fontSmall = Font::LoadFont("assets/AlteHaasGroteskRegular.ttf", 12, 32, 128);

	// Create the two teams.
	m_gameWorld.PositionBallAboveNet();
	m_gameWorld.PositionPlayersForServe();

	GameConfig config = m_gameWorld.GetConfig();

	m_selectedColorButtonIndex = -1;
	m_chooseColorButtonRadius = config.view.height * 0.05f;

	for (int i = 0; i < 2; i++)
	{
		Rect2f button(
			config.view.width * (0.25f + (i * 0.5f)),
			config.view.height * 0.5f, 0.0f, 0.0f);
		button.Inflate(
			config.view.width * 0.5f * 0.5f * 0.6f,
			config.view.height * 0.05f);
		m_joinTeamButtons.push_back(button);
	}

	for (int i = 0; i < 9; i++)
	{
		float x = ((i % 3) - 1) * config.view.height * 0.2f;
		float y = ((i / 3) - 1) * config.view.height * 0.2f;

		m_chooseColorButtons.push_back(Vector2f(
			(config.view.width * 0.5f) + x,
			(config.view.height * 0.5f) + y));
	}

	m_state = CLIENT_STATE_CHOOSE_COLOR;
}

void ClientApp::OnQuit()
{
	m_networkManager.CloseConnection();
}

void ClientApp::OnUpdate(float timeDelta)
{
	const GameConfig& config = m_gameWorld.GetConfig();

	m_inputManager.UpdateTimeStamp(timeDelta);

	ReceivePackets();

	// Escape: Quit the game.
	if (GetKeyboard()->IsKeyPressed(Keys::ESCAPE))
	{
		Quit();
		return;
	}

	if (GetKeyboard()->IsKeyPressed(Keys::PAGEUP))
		m_numMovesPerInputPacket++;
	if (GetKeyboard()->IsKeyPressed(Keys::PAGEDOWN))
		m_numMovesPerInputPacket = Math::Max(1, m_numMovesPerInputPacket - 1);
	if (GetKeyboard()->IsKeyPressed(Keys::P))
		m_enableClientSidePrediction = !m_enableClientSidePrediction;
	if (GetKeyboard()->IsKeyPressed(Keys::R))
		m_enableServerReconciliation = !m_enableServerReconciliation;
	if (GetKeyboard()->IsKeyPressed(Keys::E))
	{
		m_enableEntityInterpolation = !m_enableEntityInterpolation;
		if (!m_enableEntityInterpolation)
			m_entityInterpolator.ClearSnapshots();
	}

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	if (m_state == CLIENT_STATE_CHOOSE_COLOR)
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
			m_state = CLIENT_STATE_CHOOSE_TEAM;
		}

		m_player->SetPosition(Vector2f(config.view.width * 0.5f, config.view.height * 0.9f));
	}
	else if (m_state == CLIENT_STATE_CHOOSE_TEAM)
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
			m_state = CLIENT_STATE_PLAY_GAME;
			m_player->SetTeamIndex(m_selectedColorButtonIndex);
			m_player->SetPosition(Vector2f(
				config.view.width * (0.25f + (0.5f * m_player->GetTeamIndex())),
				config.view.floorY));

			BitStream bsOut;
			bsOut.Write((MessageID) PacketType::JOIN_TEAM);
			bsOut.Write(m_player->GetTeamIndex());
			bsOut.Write(m_player->GetColorIndex());
			m_networkManager.Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED);
			printf("Joining team %d\n", m_player->GetTeamIndex());
		}
		else
		{
			m_player->SetPosition(Vector2f(config.view.width * 0.5f, config.view.height * 0.9f));
		}
	}
	else
	{
		// Update input.
		m_inputManager.Update(timeDelta);

		// Send occasional input packets.
		if (m_inputManager.GetMoveList().GetMoveCount() >= m_numMovesPerInputPacket)
			m_networkManager.SendInputPacket();

		// Simulate/predict player movement.
		if (m_enableClientSidePrediction)
		{
			m_gameWorld.ProcessPlayerInput(m_player, m_inputManager.GetInputState(), timeDelta);
			m_gameWorld.SimulatePlayerMovement(m_player, timeDelta);
		}
	}

	// Update state interpolation (for player proxies).
	if (m_enableEntityInterpolation)
	{
		m_entityInterpolator.Update(timeDelta, &m_gameWorld);
	}
}

void ClientApp::ReceivePackets()
{
	Packet *packet;

	for (packet = m_peerInterface->Receive(); packet != NULL;
		m_peerInterface->DeallocatePacket(packet),
		packet = m_peerInterface->Receive())
	{
		BitStream inStream(packet->data, packet->length, false);
		inStream.IgnoreBytes(sizeof(MessageID));

		switch (packet->data[0])
		{
		case ID_DISCONNECTION_NOTIFICATION:
			printf("We have been disconnected.\n");
			break;
		case ID_CONNECTION_LOST:
			printf("Connection lost.\n");
			break;
		case PacketType::PLAYER_JOINED:
			ReceivePacketPlayerJoined(inStream);
			break;
		case PacketType::PLAYER_LEFT:
			ReceivePacketPlayerLeft(inStream);
			break;
		case PacketType::TEAM_SCORED:
			ReceivePacketTeamScored(inStream);
			break;
		case PacketType::TEAM_SERVE:
			ReceivePacketTeamServe(inStream);
			break;
		case PacketType::WAITING_FOR_PLAYERS:
			ReceivePacketWaitingForPlayers(inStream);
			break;
		case PacketType::UPDATE_TICK:
			ReceivePacketUpdateTick(inStream);
			break;
		}
	}
}

void ClientApp::ReceivePacketPlayerJoined(BitStream& inStream)
{
	int playerId;
	int colorIndex;
	int teamIndex;

	// Read the packet.
	inStream.Read(playerId);
	inStream.Read(teamIndex);
	inStream.Read(colorIndex);

	if (playerId != m_player->GetPlayerId())
	{
		Slime* player = m_gameWorld.GetPlayer(playerId);
		if (player == NULL)
			player = m_gameWorld.CreatePlayer(playerId);
		player->SetTeamIndex(teamIndex);
		player->SetColorIndex(colorIndex);
		player->SetJoinedGame(true);
		printf("Player ID %d joined team %d!\n", playerId, teamIndex);
	}
}

void ClientApp::ReceivePacketPlayerLeft(BitStream& inStream)
{
	int playerId;
	Color color;
	int teamIndex;

	// Read the packet.
	inStream.Read(playerId);
	inStream.Read(teamIndex);
	inStream.Read(color);

	if (playerId != m_player->GetPlayerId() && m_gameWorld.GetPlayer(playerId) != NULL)
	{
		printf("Player ID %d left the game.\n", playerId);
		m_gameWorld.RemovePlayer(playerId);
	}
}

void ClientApp::ReceivePacketTeamScored(BitStream& inStream)
{
	int scoringTeamIndex;
	inStream.Read(scoringTeamIndex);
	m_gameWorld.GetTeam(scoringTeamIndex).SetScore(
		m_gameWorld.GetTeam(scoringTeamIndex).GetScore() + 1);
	m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_SERVE);

	printf("Team %d scored!\n", scoringTeamIndex);
}

void ClientApp::ReceivePacketTeamServe(BitStream& inStream)
{
	m_gameWorld.SetState(GameWorld::STATE_GAMEPLAY);
}

void ClientApp::ReceivePacketWaitingForPlayers(BitStream& inStream)
{
	m_gameWorld.SetState(GameWorld::STATE_WAITING_FOR_PLAYERS);
}

void ClientApp::ReceivePacketUpdateTick(BitStream& inStream)
{
	Vector2f ballPos;
	Vector2f ballVel;
	bool isLastTimeStampDirty;
	float lastMoveTimeStamp;
	int lastMoveNumber;

	float lastReconciledMoveTimestamp;
	bool reconcile = false;
	MoveList& unreconciledMoves = m_inputManager.GetUnreconciledMoveList();

	// Check if there a new move response was received.
	inStream.Read(isLastTimeStampDirty);
	if (isLastTimeStampDirty)
	{
		// Read the server's last-confirmed-move's number and time stamp.
		inStream.Read(lastMoveTimeStamp);
		inStream.Read(lastMoveNumber);
		m_rtt = (m_inputManager.GetTimeStamp() - lastMoveTimeStamp);
		
		// Remove any moves from the unreconciled move list which
		// came before the server's last confirmed move number.
		for (int i = 0; i < unreconciledMoves.GetMoveCount(); i++)
		{
			if (unreconciledMoves[i].GetMoveNumber() <= lastMoveNumber)
			{
				Move m = unreconciledMoves.RemoveMove(i--);
				lastReconciledMoveTimestamp = m.GetTimeStamp();
				reconcile = true;
				if (m.GetMoveNumber() == lastMoveNumber)
					break;
			}
		}
	}
	
	// Read ball state.
	inStream.Read(ballPos);
	inStream.Read(ballVel);
	m_gameWorld.GetBall().SetPosition(ballPos);
	m_gameWorld.GetBall().SetVelocity(ballVel);

	// Read player states.
	int playerId;
	PlayerState playerState;
	Slime* player;
	
	while (true)
	{
		inStream.Read(playerId);
		if (playerId < 0)
			break;

		// Deserialize the state.
		playerState.Deserialize(inStream, m_gameWorld.GetConfig());

		// Apply the state to the associated player.
		player = m_gameWorld.GetPlayer(playerId);
		if (player != NULL)
		{
			if (player != m_player)
			{
				if (m_enableEntityInterpolation)
				{
					m_entityInterpolator.AddStateSnapshot(playerId, playerState);
				}
				else
				{
					player->SetPosition(playerState.GetPosition());
					player->SetVelocity(playerState.GetVelocity());
				}
			}
			else if (reconcile || (!m_enableServerReconciliation && isLastTimeStampDirty))
			{
				m_player->SetPosition(playerState.GetPosition());
				m_player->SetVelocity(playerState.GetVelocity());
			}
		}
	}

	// New moves were reconciled by the server.
	if (reconcile && m_enableServerReconciliation)
	{
		// Re-simulate all the unreconciled moves after the last move confirmed by the server.
		// These are the moves that happened while waiting for the server's move response.
		// There should only be 0 or 1 unreconciled moves if network communication time is instant.
		for (int i = 0; i < unreconciledMoves.GetMoveCount(); i++)
		{
			m_gameWorld.ProcessPlayerInput(m_player,
				unreconciledMoves[i].GetInputState(),
				unreconciledMoves[i].GetDeltaTime());
			m_gameWorld.SimulatePlayerMovement(m_player,
				unreconciledMoves[i].GetDeltaTime());
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

void ClientApp::DrawDebugSlime(Graphics& g, const Slime& slime, const Color& color)
{
	Polygonf slimePoly;
	int numEdges = 16;
	for (int i = 0; i <= numEdges; i++)
	{
		float angle = (i / (float)numEdges) * Math::PI;
		slimePoly.AddVertex(Vector2f(Math::Cos(angle), -Math::Sin(angle)));
	}
	
	// Draw the body.
	g.Translate(slime.GetPosition());
	g.Scale(slime.GetRadius());
	g.DrawPolygon(slimePoly, color);

	g.ResetTransform();
}

void ClientApp::OnRender()
{
	const Ball& ball = m_gameWorld.GetBall();
	const GameConfig& config = m_gameWorld.GetConfig();

	Rect2f groundRect(0, config.view.floorY, config.view.width,
		config.view.height - config.view.floorY);
	Rect2f netRect((config.view.width - config.net.width) / 2,
		config.view.floorY  - config.net.height + config.net.depthBelowGround,
		config.net.width, config.net.height);

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	// Set up a graphics drawer.
	Graphics g(GetWindow());
	g.SetViewport(Viewport(0, 0, GetWindow()->GetWidth(),
		GetWindow()->GetHeight()), false, true);
	g.SetProjection(Matrix4f::CreateOrthographic(0.0f,
		config.view.width, config.view.height, 0.0f, 1.0f, -1.0f));
	g.ResetTransform();

	// Clear the sky color.
	g.Clear(m_colorScheme.skyColor);

	// Draw the ground.
	g.FillRect(groundRect, m_colorScheme.groundColor);

	// Draw the net.
	g.FillRect(netRect, m_colorScheme.netColor);

	// Draw the ball.
	g.FillCircle(ball.GetPosition(), ball.GetRadius(), m_colorScheme.ballColor);

	// Draw the slimes.
	for (auto it = m_gameWorld.players_begin(); it != m_gameWorld.players_end(); it++)
	{
		Slime* slime = it->second;
		Vector2f lookAtPoint = ball.GetPosition();
		if (slime == m_player && (m_state == CLIENT_STATE_CHOOSE_COLOR || m_state == CLIENT_STATE_CHOOSE_TEAM))
			lookAtPoint = mousePos;
		DrawSlime(g, *slime, lookAtPoint);
	}

	// Draw state-specific stuff.
	if (m_state == CLIENT_STATE_CHOOSE_COLOR)
	{
		// Draw the prompt.
		g.DrawString(m_fontScore, "Choose a color!",
			config.view.width * 0.5f, config.view.height * 0.1f,
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
	else if (m_state == CLIENT_STATE_CHOOSE_TEAM)
	{
		// Draw the prompt.
		g.DrawString(m_fontScore, "Choose a team!",
			config.view.width * 0.5f, config.view.height * 0.1f,
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
	else if (m_state == CLIENT_STATE_PLAY_GAME || m_state == CLIENT_STATE_WAIT_FOR_SERVE)
	{
		// Draw the score.
		std::stringstream strStream;
		strStream << m_gameWorld.GetTeam(0).GetScore() << " - " << m_gameWorld.GetTeam(1).GetScore();
		g.DrawString(m_fontScore, strStream.str(),
			config.view.width * 0.5f, config.view.height * 0.1f,
			m_colorScheme.ui.scoreTextColor, Align::TOP_CENTER);
		
		// Display RTT.
		strStream.str("");
		strStream.precision(2);
		strStream << "RTT: " << m_rtt * 1000 << " ms (" << m_rtt * 60.0f << " frames)";
		g.DrawString(m_fontSmall, strStream.str(),
			16, 16, m_colorScheme.ui.scoreTextColor, Align::TOP_LEFT);
		
		// Display networking options.
		strStream.str("");
		strStream << "[PgUp/Dwn] Moves sent every " << m_numMovesPerInputPacket << " frames";
		g.DrawString(m_fontSmall, strStream.str(),
			16, 48, m_colorScheme.ui.scoreTextColor, Align::TOP_LEFT);
		g.DrawString(m_fontSmall,
			m_enableClientSidePrediction ? "[P] Prediction: ON" : "[P] Prediction: OFF",
			16, 64, m_colorScheme.ui.scoreTextColor, Align::TOP_LEFT);
		g.DrawString(m_fontSmall,
			m_enableServerReconciliation ? "[R] Reconciliation: ON" : "[R] Reconciliation: OFF",
			16, 80, m_colorScheme.ui.scoreTextColor, Align::TOP_LEFT);
		g.DrawString(m_fontSmall,
			m_enableEntityInterpolation ? "[E] Entity Interpolation: ON" : "[E] Entity Interpolation: OFF",
			16, 96, m_colorScheme.ui.scoreTextColor, Align::TOP_LEFT);

		// Count the number of players per team.
		int playerCounts[2] = { 0, 0 };
		for (auto it = m_gameWorld.players_begin(); it != m_gameWorld.players_end(); it++)
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

