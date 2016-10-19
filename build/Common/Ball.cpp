#include "Ball.h"


Ball::Ball(const Vector3f& color, float radius, const Vector2f& position) :
	m_color(color),
	m_radius(radius),
	m_position(position),
	m_velocity(Vector2f::ZERO)
{
}

void Ball::SetPosition(const Vector2f& position)
{
	m_position = position;
}

void Ball::SetVelocity(const Vector2f& velocity)
{
	m_velocity = velocity;
}

void Ball::SetColor(const Vector3f& color)
{
	m_color = color;
}

void Ball::SetRadius(float radius)
{
	m_radius = radius;
}

const Vector2f& Ball::GetPosition() const
{
	return m_position;
}

const Vector2f& Ball::GetVelocity() const
{
	return m_velocity;
}

const Vector3f& Ball::GetColor() const
{
	return m_color;
}

float Ball::GetRadius() const
{
	return m_radius;
}
