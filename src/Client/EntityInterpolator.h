#ifndef _ENTITY_INTERPOLATOR_H_
#define _ENTITY_INTERPOLATOR_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>

#include "InputManager.h"
#include "GameWorld.h"
#include "WorldState.h"
#include "PlayerState.h"


class EntityStateHistory
{
public:
	EntityStateHistory(int entityId);

	inline int GetEntityId() const { return m_entityId; }
	inline const EntityState& GetState() const { return m_state; }

	void AddState(float interpolationDelay, float timeStamp, const Vector2f& position, const Vector2f& velocity);
	void ClearBuffer();
	void Update(float timeDelta, float interpolationDelay);
	
private:
	EntityState m_state;
	std::vector<EntityState> m_buffer;
	float m_lastTimeStamp;
	int m_entityId;
};


class EntityInterpolator
{
public:
	EntityInterpolator();
	~EntityInterpolator();

	inline const WorldState& GetWorldState() const { return m_interpolatedState; }
	EntityStateHistory* GetEntityStateHistory(int entityId);

	void AddStateSnapshots(WorldState& worldState);
	void AddStateSnapshot(int playerId, const PlayerState& playerState);
	void AddStateSnapshot(int entityId, float timeStamp, const Vector2f& position, const Vector2f& velocity);
	void ClearSnapshots();
	void Update(float timeDelta, GameWorld* gameWorld);

private:
	WorldState m_interpolatedState;
	std::vector<WorldState*> m_snapshots;
	float m_lastSnapshotTimeStamp;
	float m_interpolationDelay;

	typedef std::map<int, EntityStateHistory*> EntityStateHistoryMap;

	EntityStateHistoryMap m_entityStateHistoryMap;
};


#endif // _ENTITY_INTERPOLATOR_H_