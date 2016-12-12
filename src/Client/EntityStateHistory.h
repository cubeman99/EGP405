#ifndef _ENTITY_STATE_HISTORY_H_
#define _ENTITY_STATE_HISTORY_H_

#include <vector>
#include "EntityState.h"


// A history of states for a single entity that can be interpolated between.
class EntityStateHistory
{
public:
	EntityStateHistory(int entityId);

	inline int GetEntityId() const { return m_entityId; }
	inline const EntityState& GetState() const { return m_state; }

	void AddState(float interpolationDelay, float timeStamp,
		const Vector2f& position, const Vector2f& velocity);
	void ClearBuffer();
	void Update(float timeDelta, float interpolationDelay);
	
private:
	EntityState m_state;
	std::vector<EntityState> m_buffer;
	float m_lastTimeStamp;
	int m_entityId;
};


#endif // _ENTITY_STATE_HISTORY_H_