#ifndef _BALL_H_
#define _BALL_H_


class Ball
{
public:
	Ball(const Vector3f& color, float radius, const Vector2f& position);

	void SetPosition(const Vector2f& position);
	void SetVelocity(const Vector2f& velocity);
	void SetColor(const Vector3f& color);
	void SetRadius(float radius);
	
	const Vector2f& GetPosition() const;
	const Vector2f& GetVelocity() const;
	const Vector3f& GetColor() const;
	float GetRadius() const;
	
	// TODO: serialize/deserialize

private:
	Vector2f m_position;
	Vector2f m_velocity;
	Vector3f m_color;
	float m_radius;
};


#endif // _BALL_H_