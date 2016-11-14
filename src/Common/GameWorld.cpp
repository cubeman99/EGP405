#include "GameWorld.h"
#include <math/MathLib.h>

GameWorld::GameWorld()
{
}

GameWorld::~GameWorld()
{
	// Delete all players.
	for (PlayerMap::iterator it = m_players.begin(); it != m_players.end(); it++)
	{
		delete it->second;
	}

	m_players.clear();
}


Slime* GameWorld::GetPlayer(PlayerID playerId)
{
	PlayerMap::iterator it = m_players.find(playerId);
	if (it != m_players.end())
		return it->second;
	return NULL;
}

Slime* GameWorld::CreatePlayer(PlayerID playerId)
{
	Slime* slime = new Slime();
	slime->SetPlayerId(playerId);
	m_players[playerId] = slime;
	return slime;
}

void GameWorld::RemovePlayer(PlayerID playerId)
{
	m_players.erase(playerId);
}

void GameWorld::SimulatePlayerMovement(Slime* player, float deltaTime)
{
	Vector2f position = player->GetPosition();
	Vector2f velocity = player->GetVelocity();
	Team* team = &m_teams[player->GetTeamIndex()];
	float minX = team->GetPlayRegion().GetLeft();
	float maxX = team->GetPlayRegion().GetRight();
	float radius = player->GetRadius();

	// Integrate gravitational force.
	velocity.y += (m_config.slime.gravity * 60.0f) * deltaTime;

	// Collide with floor and walls.
	if (position.y >= m_config.view.floorY)
	{
		position.y = m_config.view.floorY;
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

void GameWorld::SimulateBallMovement(float deltaTime)
{
	Vector2f position = m_ball.GetPosition();
	Vector2f velocity = m_ball.GetVelocity();

	velocity.y += m_config.ball.gravity;
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
				velocity.y -= m_config.ball.gravity * 0.5f;

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
		Vector2f v1(m_config.view.width * 0.5f, m_config.view.floorY -
			m_config.net.height + m_config.net.depthBelowGround);
		Vector2f v2(m_config.view.width * 0.5f, m_config.view.floorY +
			m_config.net.depthBelowGround);

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
	else if (position.x + m_ball.GetRadius() >= m_config.view.width)
	{
		position.x = m_config.view.width - m_ball.GetRadius();
		if (velocity.x > 0.0f)
			velocity.x = -velocity.x;
	}
	if (position.y + m_ball.GetRadius() >= m_config.view.floorY)
	{
		position.y = m_config.view.floorY - m_ball.GetRadius();
		velocity = Vector2f::ZERO;

		OnTeamScore(position.x < m_config.view.width * 0.5f ? 1 : 0);
		//if (velocity.y > 0.0f)
		//velocity.y = -velocity.y;
	}

	// Limit max speed.
	float velLength = velocity.Length();
	if (velLength > m_config.ball.maxSpeed)
		velocity *= m_config.ball.maxSpeed / velLength;

	m_ball.SetPosition(position);
	m_ball.SetVelocity(velocity);
}

void GameWorld::OnTeamScore(int teamIndex)
{
	m_state = STATE_WAITING_FOR_SERVE;
	if (m_players.size() < 2)
		m_state = STATE_WAITING_FOR_SERVE;
}
