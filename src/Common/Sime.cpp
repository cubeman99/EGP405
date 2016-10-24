#include "Slime.h"


Slime::Slime() :
	m_velocity(Vector2f::ZERO),
	m_teamIndex(0),
	m_playerId(-1),
	m_joinedGame(false),
	m_name("ERROR")
{
}

Slime::Slime(const Color& color, float radius, const Vector2f& position) :
	m_color(color),
	m_radius(radius),
	m_position(position),
	m_velocity(Vector2f::ZERO),
	m_teamIndex(0),
	m_playerId(-1),
	m_joinedGame(false),
	m_name("ERROR")
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

void Slime::SetColor(const Color& color)
{
	m_color = color;
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

void Slime::SetName(const std::string& name)
{
	m_name = name;
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

const Color& Slime::GetColor() const
{
	return m_color;
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

const std::string& Slime::GetName() const
{
	return m_name;
}

bool Slime::HasJoinedGame() const
{
	return m_joinedGame;
}
