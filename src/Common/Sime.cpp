#include "Slime.h"
#include <math/MathLib.h>


Slime::Slime() :
	m_velocity(Vector2f::ZERO),
	m_teamIndex(0),
	m_playerId(-1),
	m_joinedGame(false)
{
}

Slime::Slime(int playerId, int teamIndex, int colorIndex, float radius) :
	m_colorIndex(colorIndex),
	m_teamIndex(teamIndex),
	m_playerId(playerId),
	m_radius(radius),
	m_position(Vector2f::ZERO),
	m_velocity(Vector2f::ZERO),
	m_joinedGame(false)
{
}

void Slime::SetPosition(const Vector2f& position)
{
	m_position = position;
}

void Slime::SetVelocity(const Vector2f& velocity)
{
	m_velocity = velocity;
}

void Slime::SetColorIndex(int colorIndex)
{
	m_colorIndex = colorIndex;
}

void Slime::SetRadius(float radius)
{
	m_radius = radius;
}

void Slime::SetTeamIndex(int teamIndex)
{
	m_teamIndex = teamIndex;
}

void Slime::SetPlayerId(int playerId)
{
	m_playerId = playerId;
}

void Slime::SetJoinedGame(bool isJoined)
{
	m_joinedGame = isJoined;
}


const Vector2f& Slime::GetPosition() const
{
	return m_position;
}

const Vector2f& Slime::GetVelocity() const
{
	return m_velocity;
}

int Slime::GetColorIndex() const
{
	return m_colorIndex;
}

float Slime::GetRadius() const
{
	return m_radius;
}

int Slime::GetTeamIndex() const
{
	return m_teamIndex;
}

int Slime::GetPlayerId() const
{
	return m_playerId;
}

bool Slime::HasJoinedGame() const
{
	return m_joinedGame;
}


void Slime::SerializeDynamics(RakNet::BitStream& bsOut, const GameConfig& gameConfig) const
{
	bool isOnGround = (Math::Abs(m_position.y - gameConfig.view.floorY) < 0.01f);

	// X position.
	// (never compres).
	bsOut.Write(m_position.x);

	// Y position.
	// Compress if on the ground.
	if (!isOnGround)
	{
		bsOut.Write0();
		bsOut.Write(m_position.y);
	}
	else
	{
		bsOut.Write1();
	}

	// Velocity.
	// Component-wise compress if zero.
	for (int i = 0; i < 2; i++)
	{
		if (m_velocity[i] != 0.0f)
		{
			bsOut.Write0();
			bsOut.Write(m_velocity[i]);
		}
		else
		{
			bsOut.Write1();
		}
	}
}

void Slime::DeserializeDynamics(RakNet::BitStream& bsIn, const GameConfig& gameConfig)
{
	// X position.
	// (never compressed).
	bsIn.Read(m_position.x);

	// Y position.
	// Compressed if on the ground.
	if (bsIn.ReadBit())
	{
		m_position.y = gameConfig.view.floorY;
	}
	else
	{
		bsIn.Read(m_position.y);
	}

	// Velocity.
	// Component-wise compressed if zero.
	for (int i = 0; i < 2; i++)
	{
		if (bsIn.ReadBit())
		{
			m_velocity[i] = 0.0f;
		}
		else
		{
			bsIn.Read(m_velocity[i]);
		}
	}
}
