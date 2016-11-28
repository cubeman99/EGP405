#include "ClientProxy.h"


ClientProxy::ClientProxy(const RakNet::SystemAddress& socketAddress, int playerId) :
	m_socketAddress(socketAddress),
	m_playerId(playerId),
	m_isLastMoveTimestampDirty(false)
{

}