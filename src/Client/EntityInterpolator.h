#ifndef _ENTITY_INTERPOLATOR_H_
#define _ENTITY_INTERPOLATOR_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>

#include "InputManager.h"
#include "PlayerProxy.h"
#include "GameWorld.h"
#include "WorldState.h"


class EntityInterpolator
{
public:
	EntityInterpolator();
	~EntityInterpolator();

	WorldState* CreateSnapshotIfNew(float timeStamp);

	void ClearSnapshots();

	inline const WorldState& GetWorldState() const { return m_interpolatedState; }

	void Update(float timeDelta, GameWorld* gameWorld);

private:
	WorldState m_interpolatedState;
	std::vector<WorldState*> m_snapshots;
	float m_lastSnapshotTimeStamp;
	float m_interpolationDelay;
};


#endif // _ENTITY_INTERPOLATOR_H_