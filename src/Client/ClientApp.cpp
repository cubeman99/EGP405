#include "ClientApp.h"
#include <GameLib/graphics/Graphics.h>
#include <Common/Config.h>
#include <math/MathLib.h>

ClientApp::ClientApp()
{

}

void ClientApp::OnInitialize()
{
	m_ball = Ball(m_config.BALL_COLOR, m_config.BALL_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.25f,
			m_config.FLOOR_Y - m_config.BALL_SERVE_HEIGHT));

	
	m_slimes.push_back(Slime(Color::RED, m_config.SLIME_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.25f,
			m_config.FLOOR_Y)));
	m_slimes.push_back(Slime(Color::GREEN, m_config.SLIME_RADIUS,
		Vector2f(m_config.VIEW_WIDTH * 0.75f,
			m_config.FLOOR_Y)));
	m_slimes[0].SetTeamIndex(0);
	m_slimes[1].SetTeamIndex(1);

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
	if (GetKeyboard()->IsKeyPressed(Keys::ESCAPE))
	{
		Quit();
		return;
	}

	Vector2f mousePos((float) GetMouse()->GetX(), (float) GetMouse()->GetY());

	Slime* player = &m_slimes[0];

	if (m_state == STATE_CHOOSE_COLOR)
	{
		m_selectedColorButtonIndex = -1;

		// Check for hovering over color option buttons.
		for (int i = 0; i < m_config.NUM_SLIME_COLORS; i++)
		{
			if (mousePos.DistTo(m_chooseColorButtons[i]) < m_chooseColorButtonRadius * 1.2f)
			{
				m_selectedColorButtonIndex = i;
				player->SetColor(m_config.SLIME_COLORS[i]);
			}
		}

		// Check for clicking on a color option button.
		if (m_selectedColorButtonIndex >= 0 &&
			GetMouse()->IsButtonPressed(MouseButtons::LEFT))
		{
			m_state = STATE_CHOOSE_TEAM;
		}

		player->SetPosition(Vector2f(m_config.VIEW_WIDTH * 0.5f, m_config.VIEW_HEIGHT * 0.9f));
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
				player->SetTeamIndex(1 - i);
			}
		}

		// Check for clicking on a join team button.
		if (m_selectedColorButtonIndex >= 0 &&
			GetMouse()->IsButtonPressed(MouseButtons::LEFT))
		{
			m_state = STATE_PLAY_GAME;
			player->SetTeamIndex(m_selectedColorButtonIndex);
			player->SetPosition(Vector2f(
				m_config.VIEW_WIDTH * (0.25f + (0.5f * player->GetTeamIndex())),
				m_config.FLOOR_Y));
		}
		else
		{
			player->SetPosition(Vector2f(m_config.VIEW_WIDTH * 0.5f, m_config.VIEW_HEIGHT * 0.9f));
		}
	}
	else if (m_state == STATE_PLAY_GAME)
	{
		UpdatePlayer();
		UpdateBall();
	}
	else if (m_state == STATE_WAIT_FOR_SERVE)
	{

	}
}

void ClientApp::UpdateBall()
{
	Vector2f position = m_ball.GetPosition();
	Vector2f velocity = m_ball.GetVelocity();

	velocity.y += m_config.BALL_GRAVITY;
	position += velocity;

	// Collide with slimes.
	for (unsigned int i = 0; i < m_slimes.size(); i++)
	{
		Slime& slime = m_slimes[i];

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

void ClientApp::UpdatePlayer()
{
	Slime* player = &m_slimes[0];
	Vector2f pos = player->GetPosition();
	Vector2f vel = player->GetVelocity();

	float minX;
	float maxX;

	if (player->GetTeamIndex() == 0)
	{
		minX = 0.0f;
		maxX = (m_config.VIEW_WIDTH - m_config.NET_WIDTH) * 0.5f;
	}
	else
	{
		minX = (m_config.VIEW_WIDTH + m_config.NET_WIDTH) * 0.5f;
		maxX = m_config.VIEW_WIDTH;
	}

	if (GetKeyboard()->IsKeyDown(Keys::LEFT))
		pos.x -= 4.5f;
	if (GetKeyboard()->IsKeyDown(Keys::RIGHT))
		pos.x += 4.5f;
	if (GetKeyboard()->IsKeyPressed(Keys::UP) && pos.y >= m_config.FLOOR_Y)
		vel.y = -m_config.SLIME_JUMP_SPEED;

	vel.y += m_config.SLIME_GRAVITY;
	pos += vel;

	if (pos.y >= m_config.FLOOR_Y)
	{
		pos.y = m_config.FLOOR_Y;
		vel.y = 0;
	}
	if (pos.x - player->GetRadius() <= minX)
	{
		pos.x = minX + player->GetRadius();
		vel.x = 0;
	}
	else if (pos.x + player->GetRadius() >= maxX)
	{
		pos.x = maxX - player->GetRadius();
		vel.x = 0;
	}

	player->SetPosition(pos);
	player->SetVelocity(vel);
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
	for (unsigned int i = 0; i < m_slimes.size(); i++)
	{
		Vector2f lookAtPoint = m_ball.GetPosition();
		if (i == 0 && (m_state == STATE_CHOOSE_COLOR || m_state == STATE_CHOOSE_TEAM))
			lookAtPoint = mousePos;
		DrawSlime(g, m_slimes[i], lookAtPoint);
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