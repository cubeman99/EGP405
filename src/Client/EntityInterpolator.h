#ifndef _ENTITY_INTERPOLATOR_H_
#define _ENTITY_INTERPOLATOR_H_

#include "GameWorld.h" 
#include "PlayerState.h"
#include "EntityStateHistory.h"


// This class keeps a history of states for entities (currently players)
// so they can be interpolated between.
class EntityInterpolator
{
public:
	EntityInterpolator();
	~EntityInterpolator();

	EntityStateHistory* GetEntityStateHistory(int entityId);

	void AddStateSnapshot(int playerId, const PlayerState& playerState);
	void AddStateSnapshot(int entityId, float timeStamp, const Vector2f& position, const Vector2f& velocity);
	void ClearSnapshots();
	void Update(float timeDelta, GameWorld* gameWorld);

private:
	float m_interpolationDelay;

	typedef std::map<int, EntityStateHistory*> EntityStateHistoryMap;

	EntityStateHistoryMap m_entityStateHistoryMap;
};


#endif // _ENTITY_INTERPOLATOR_H_