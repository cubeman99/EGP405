#ifndef _PLAYER_STATE_H_
#define _PLAYER_STATE_H_

#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakPeerInterface.h>
#include <GameLib/math/Vector2f.h>
#include "Config.h"

class PlayerState
{
public:
	PlayerState()
	{
	}

	PlayerState(float timeStamp, const Vector2f& position, const Vector2f& velocity) :
		m_timeStamp(timeStamp),
		m_position(position),
		m_velocity(velocity)
	{
	}

	inline float GetTimeStamp() const { return m_timeStamp; }
	inline const Vector2f& GetPosition() const { return m_position; }
	inline const Vector2f& GetVelocity() const { return m_velocity; }

	inline void SetTimeStamp(float timeStamp) { m_timeStamp = timeStamp; }
	inline void SetPosition(const Vector2f& position) { m_position = position; }
	inline void SetVelocity(const Vector2f& velocity) { m_velocity = velocity; }

	void Serialize(RakNet::BitStream& bsOut, const GameConfig& gameConfig) const;
	void Deserialize(RakNet::BitStream& bsIn, const GameConfig& gameConfig);

private:
	float m_timeStamp;
	Vector2f m_position;
	Vector2f m_velocity;
};


#endif // _PLAYER_STATE_H_