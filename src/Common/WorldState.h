#ifndef _WORLD_STATE_H_
#define _WORLD_STATE_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <GameLib/math/Vector2f.h>
#include <vector>
#include <map>


class EntityState
{
public:
	EntityState()
	{
	}

	EntityState(int entityId, const Vector2f& position, const Vector2f& velocity) :
		m_entityId(entityId),
		m_position(position),
		m_velocity(velocity)
	{
	}

	inline int GetEntityId() const { return m_entityId; }
	inline const Vector2f& GetPosition() const { return m_position; }
	inline const Vector2f& GetVelocity() const { return m_velocity; }

	inline void SetEntityId(int entityId) { m_entityId = entityId; }
	inline void SetPosition(const Vector2f& position) { m_position = position; }
	inline void SetVelocity(const Vector2f& velocity) { m_velocity = velocity; }

	void Serialize(RakNet::BitStream& outStream) const;
	void Deserialize(RakNet::BitStream& inStream);

private:
	int m_entityId;
	Vector2f m_position;
	Vector2f m_velocity;
};


class WorldState
{
public:
	typedef std::map<int, EntityState> EntityIdToEntityStateMap;

public:
	WorldState();
	WorldState(float timeStamp);
	
	void Clear();

	inline float GetTimeStamp() const { return m_timeStamp; }

	inline void SetTimeStamp(float timeStamp) { m_timeStamp = timeStamp; }

	inline EntityIdToEntityStateMap::iterator entity_states_begin() { return m_entityStates.begin(); }
	inline EntityIdToEntityStateMap::iterator entity_states_end() { return m_entityStates.end(); }

	void Interpolate(WorldState* a, WorldState* b, float t);
	void Set(WorldState* other);

	EntityState* GetEntityState(int entityId);
	void SetEntityState(const EntityState& entityState);

	void Serialize(RakNet::BitStream& outStream) const;
	void Deserialize(RakNet::BitStream& inStream);

private:
	float m_timeStamp;
	EntityIdToEntityStateMap m_entityStates;
};


#endif // _WORLD_STATE_H_