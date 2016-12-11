#ifndef _NETWORK_MANAGER_CLIENT_H_
#define _NETWORK_MANAGER_CLIENT_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>

#include "InputManager.h"
#include "PlayerProxy.h"
#include "GameWorld.h"


struct EntityState
{
	int entityId;
	Vector2f position;
	Vector2f velocity;
};

class Snapshot
{
public:

private:
	std::vector<EntityState> m_entityStates;
};



class NetworkManagerClient
{
public:
	NetworkManagerClient();
	~NetworkManagerClient();

	void Initialize(RakNet::RakPeerInterface* peerInterface,
		InputManager* inputManager, const RakNet::SystemAddress& serverAddress);

	void SendInputPacket();

	void Send(const RakNet::BitStream* bitStream,
		PacketPriority priority, PacketReliability reliability);

	void CloseConnection();

	void AddPlayerProxy(PlayerID playerId);
	void RemovePlayerProxy(PlayerID playerId);
	PlayerProxy* GetPlayerProxy(PlayerID playerId);

	void UpdatePlayerProxies(float timeDelta);

private:
	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SystemAddress m_serverAddress;

	InputManager* m_inputManager;

	float m_avgRoundTripTime;

	std::map<PlayerID, PlayerProxy*> m_playerIdToPlayerProxyMap;
};


#endif // _NETWORK_MANAGER_CLIENT_H_