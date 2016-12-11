#ifndef _PLAYER_PROXY_H_
#define _PLAYER_PROXY_H_

#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakPeerInterface.h>
#include "Slime.h"
#include "GameWorld.h"
#include "PlayerState.h"


class PlayerProxy
{
public:
	PlayerProxy(PlayerID playerId);

	void AddStateIfNew(float timeStamp, const Vector2f& position, const Vector2f& velocity);

	void ClearInterpolation();
	void UpdateInterpolation(float timeDelta);

	inline const PlayerState& GetPlayerState() const { return m_interpolationState; }

private:
	PlayerID m_playerId;
	PlayerState m_interpolationState;
	std::vector<PlayerState> m_stateQueue;
	float m_lastStateTimeStamp;


	float m_interpolationDelay;
};



#endif // _PLAYER_PROXY_H_