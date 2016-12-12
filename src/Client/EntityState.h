#ifndef _ENTITY_STATE_H_
#define _ENTITY_STATE_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <GameLib/math/Vector2f.h>


class EntityState
{
public:
	EntityState();
	EntityState(float timeStamp, int entityId, const Vector2f& position, const Vector2f& velocity);
	EntityState(int entityId, const Vector2f& position, const Vector2f& velocity);

	inline int GetEntityId() const { return m_entityId; }
	inline float GetTimeStamp() const { return m_timeStamp; }
	inline const Vector2f& GetPosition() const { return m_position; }
	inline const Vector2f& GetVelocity() const { return m_velocity; }

	inline void SetEntityId(int entityId) { m_entityId = entityId; }
	inline void SetTimeStamp(float timeStamp) { m_timeStamp = timeStamp; }
	inline void SetPosition(const Vector2f& position) { m_position = position; }
	inline void SetVelocity(const Vector2f& velocity) { m_velocity = velocity; }

	void Serialize(RakNet::BitStream& outStream) const;
	void Deserialize(RakNet::BitStream& inStream);

private:
	int m_entityId;
	float m_timeStamp;
	Vector2f m_position;
	Vector2f m_velocity;
};


#endif // _ENTITY_STATE_H_