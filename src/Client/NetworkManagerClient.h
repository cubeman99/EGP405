#ifndef _NETWORK_MANAGER_CLIENT_H_
#define _NETWORK_MANAGER_CLIENT_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>

#include "InputManager.h"


class NetworkManagerClient
{
public:
	NetworkManagerClient();
	
	void Initialize(RakNet::RakPeerInterface* peerInterface,
		InputManager* inputManager, const RakNet::SystemAddress& serverAddress);

	void SendInputPacket();

	void Send(const RakNet::BitStream* bitStream,
		PacketPriority priority, PacketReliability reliability);

	void CloseConnection();

private:
	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SystemAddress m_serverAddress;

	InputManager* m_inputManager;

	float m_avgRoundTripTime;
};


#endif // _NETWORK_MANAGER_CLIENT_H_