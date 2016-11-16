#ifndef _NETWORK_MANAGER_SERVER_H_
#define _NETWORK_MANAGER_SERVER_H_

#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>
#include <RakNet/RakPeerInterface.h>

#include <map>

#include "ClientProxy.h"


typedef unsigned char NetworkEventID;

class NetworkEventHandler
{
public:
	virtual void OnClientConnect(ClientProxy* client) = 0;
	virtual void OnClientDisconnect(ClientProxy* client) = 0;
	virtual void OnClientEvent(ClientProxy* client, NetworkEventID eventId, RakNet::BitStream& inStream) = 0;
};


class NetworkManagerServer
{
private:
	typedef std::map<RakNet::SystemAddress, ClientProxy*> AddressToClientProxyMap;
	typedef std::map<int, ClientProxy*> PlayerIDToClientProxyMap;

public:
	NetworkManagerServer();
	~NetworkManagerServer();

	bool Initialize(unsigned short port, unsigned int maxPlayers, NetworkEventHandler* eventHandler);

	void OnClientConnect(const RakNet::SystemAddress& address);
	void OnClientDisconnect(ClientProxy* client);

	void ReceivePackets();
	void ReceivePacket(RakNet::Packet* packet);
	void ReceiveInputPacket(ClientProxy* client, RakNet::BitStream& inputPacket);
	
	ClientProxy* GetClientProxy(int playerId);

	PlayerIDToClientProxyMap::iterator clients_begin();
	PlayerIDToClientProxyMap::iterator clients_end();

	void Broadcast(const RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, const ClientProxy* ignoreClient = NULL);
	void Send(const RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, const ClientProxy* client);

private:

	int m_playerIdCounter;
	AddressToClientProxyMap m_addressToClientProxyMap;
	PlayerIDToClientProxyMap m_playerIdToClientProxyMap;
	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SystemAddress m_serverAddress;
	NetworkEventHandler* m_eventHandler;
};


#endif // _NETWORK_MANAGER_SERVER_H_