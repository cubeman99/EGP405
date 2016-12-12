#ifndef _BALL_H_
#define _BALL_H_

#include <math/Vector2f.h>
#include <graphics/Color.h>


class Ball
{
public:
	Ball();
	Ball(float radius, const Vector2f& position);

	void SetPosition(const Vector2f& position);
	void SetVelocity(const Vector2f& velocity);
	void SetRadius(float radius);
	
	const Vector2f& GetPosition() const;
	const Vector2f& GetVelocity() const;
	float GetRadius() const;

private:
	Vector2f m_position;
	Vector2f m_velocity;
	float m_radius;
};


#endif // _BALL_H_