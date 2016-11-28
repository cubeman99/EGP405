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

	inline void SetIsLastMoveTimestampDirty(bool isLastMoveTimestampDirty)
		{ m_isLastMoveTimestampDirty = isLastMoveTimestampDirty; }

private:
	RakNet::SystemAddress m_socketAddress;
	int			m_playerId;
	MoveList	m_unprocessedMoveList;
	bool		m_isLastMoveTimestampDirty;
};



#endif // _CLIENT_PROXY_H_