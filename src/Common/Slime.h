#ifndef _SLIME_H_
#define _SLIME_H_

#include <math/Vector2f.h>
#include <graphics/Color.h>
#include <RakNet/BitStream.h>
#include "GameConfig.h"
#include <string>


// The appearence and state information for a slime entity,
// representing a player in the game.
class Slime
{
public:
	Slime();
	Slime(int playerId, int teamIndex, int colorIndex, float radius);

	void SetPosition(const Vector2f& position);
	void SetVelocity(const Vector2f& velocity);
	void SetColorIndex(int colorIndex);
	void SetRadius(float radius);
	void SetTeamIndex(int teamIndex);
	void SetPlayerId(int playerId);
	void SetJoinedGame(bool hasJoined);

	const Vector2f& GetPosition() const;
	const Vector2f& GetVelocity() const;
	int GetColorIndex() const;
	float GetRadius() const;
	int GetTeamIndex() const;
	int GetPlayerId() const;
	bool HasJoinedGame() const;
	
	void SerializeDynamics(RakNet::BitStream& bsOut, const GameConfig& gameConfig) const;
	void DeserializeDynamics(RakNet::BitStream& bsIn, const GameConfig& gameConfig);

private:
	Vector2f	m_position;
	Vector2f	m_velocity;
	int			m_playerId;
	int			m_colorIndex;
	int			m_teamIndex;
	float		m_radius;
	bool		m_joinedGame; // for use by server.
};


#endif // _SLIME_H_