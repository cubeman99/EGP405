#ifndef _CLIENT_PROXY_H_
#define _CLIENT_PROXY_H_

#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakPeerInterface.h>

#include "MoveList.h"


class ClientProxy
{
public:
	ClientProxy(const RakNet::SystemAddress& socketAddress, int playerId);

	inline MoveList& GetUnprocessedMoveList() { return m_unprocessedMoveList; }

	inline const RakNet::SystemAddress& GetSocketAddress() const { return m_socketAddress; }
	inline int GetPlayerId() const { return m_playerId; }
	inline int GetLastMoveNumber() const { return m_lastMoveNumber; }
	inline float GetLastMoveTimeStamp() const { return m_lastMoveTimeStamp; }
	inline bool IsLastMoveTimestampDirty() const { return m_isLastMoveTimestampDirty; }

	inline void SetLastMoveTimeStamp(float lastMoveTimeStamp) { m_lastMoveTimeStamp = lastMoveTimeStamp; }
	inline void SetIsLastMoveTimestampDirty(bool isLastMoveTimestampDirty)
		{ m_isLastMoveTimestampDirty = isLastMoveTimestampDirty; }


private:
	RakNet::SystemAddress m_socketAddress;
	int			m_playerId;
	MoveList	m_unprocessedMoveList;
	int			m_lastMoveNumber;
	bool		m_isLastMoveTimestampDirty;
	float		m_lastMoveTimeStamp;
};


#endif // _CLIENT_PROXY_H_