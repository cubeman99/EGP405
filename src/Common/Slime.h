#ifndef _SLIME_H_
#define _SLIME_H_

#include <math/Vector2f.h>
#include <graphics/Color.h>
#include <string>


class Slime
{
public:
	Slime();
	Slime(const Color& color, float radius, const Vector2f& position);

	void SetPosition(const Vector2f& position);
	void SetVelocity(const Vector2f& velocity);
	void SetColor(const Color& color);
	void SetRadius(float radius);
	void SetTeamIndex(int teamIndex);
	void SetPlayerId(int playerId);
	void SetName(const std::string& name);
	void SetJoinedGame(bool hasJoined);

	const Vector2f& GetPosition() const;
	const Vector2f& GetVelocity() const;
	const Color& GetColor() const;
	float GetRadius() const;
	int GetTeamIndex() const;
	int GetPlayerId() const;
	const std::string& GetName() const;
	bool HasJoinedGame() const;
	
	// TODO: serialize/deserialize

private:
	Vector2f m_position;
	Vector2f m_velocity;
	Color m_color;
	float m_radius;
	int m_teamIndex;
	int m_playerId;
	bool m_joinedGame;
	std::string m_name;
};


#endif // _SLIME_H_