#include "Ball.h"

Ball::Ball()
{
}

Ball::Ball(float radius, const Vector2f& position) :
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

float Ball::GetRadius() const
{
	return m_radius;
}
