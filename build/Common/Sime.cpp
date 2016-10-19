#include "Slime.h"


Slime::Slime(const Vector3f& color, float radius, const Vector2f& position) :
	m_color(color),
	m_radius(radius),
	m_position(position),
	m_velocity(Vector2f::ZERO)
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

void Slime::SetColor(const Vector3f& color)
{
	m_color = color;
}

void Slime::SetRadius(float radius)
{
	m_radius = radius;
}

const Vector2f& Slime::GetPosition() const
{
	return m_position;
}

const Vector2f& Slime::GetVelocity() const
{
	return m_velocity;
}

const Vector3f& Slime::GetColor() const
{
	return m_color;
}

float Slime::GetRadius() const
{
	return m_radius;
}
