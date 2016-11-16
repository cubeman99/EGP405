#include "NetworkManagerServer.h"
#include "Messages.h"

using namespace RakNet;



NetworkManagerServer::NetworkManagerServer() :
	m_peerInterface(NULL),
	m_eventHandler(NULL)
{
}

NetworkManagerServer::~NetworkManagerServer()
{
	for (auto it = m_addressToClientProxyMap.begin();
		it != m_addressToClientProxyMap.end(); it++)
	{
		delete it->second;
	}
	m_addressToClientProxyMap.clear();
	m_playerIdToClientProxyMap.clear();
}

bool NetworkManagerServer::Initialize(unsigned short port, unsigned int maxPlayers, NetworkEventHandler* eventHandler)
{
	m_eventHandler = eventHandler;
	m_playerIdCounter = 1;
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();

	// Start the RakNet Peer Interface
	SocketDescriptor sd(port, 0);
	StartupResult result = m_peerInterface->Startup(maxPlayers, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(maxPlayers);
	m_peerInterface->SetOccasionalPing(true);

	if (result != StartupResult::RAKNET_STARTED)
	{
		printf("Error: failure to start the RakNet peer interface.\n");
		return false;
	}

	return true;
}

void NetworkManagerServer::OnClientConnect(const RakNet::SystemAddress& address)
{
	// Create a new client proxy.
	ClientProxy* client = new ClientProxy(address, m_playerIdCounter);
	m_addressToClientProxyMap[address] = client;
	m_playerIdToClientProxyMap[m_playerIdCounter] = client;
	m_playerIdCounter++;

	// Notify the event handler.
	m_eventHandler->OnClientConnect(client);
}

void NetworkManagerServer::OnClientDisconnect(ClientProxy* client)
{
	// Notify the event handler.
	m_eventHandler->OnClientDisconnect(client);

	// Delete the cleint proxy.
	m_addressToClientProxyMap.erase(client->GetSocketAddress());
	m_playerIdToClientProxyMap.erase(client->GetPlayerId());
	delete client;
}

void NetworkManagerServer::ReceivePackets()
{
	for (Packet *packet = m_peerInterface->Receive(); packet != NULL;
		m_peerInterface->DeallocatePacket(packet),
		packet = m_peerInterface->Receive())
	{
		ReceivePacket(packet);
	}
}

void NetworkManagerServer::ReceivePacket(RakNet::Packet* packet)
{
	MessageID packetType = packet->data[0];
	ClientProxy* client = NULL;

	// Find the Client Proxy for this address.
	auto it = m_addressToClientProxyMap.find(packet->systemAddress);
	if (it == m_addressToClientProxyMap.end())
	{
		if (packetType == ID_REMOTE_NEW_INCOMING_CONNECTION ||
			packetType == ID_NEW_INCOMING_CONNECTION)
		{
			OnClientConnect(packet->systemAddress);
		}
		return;
	}
	else
	{
		client = it->second;
	}

	BitStream inStream(packet->data, packet->length, false);
	inStream.IgnoreBytes(sizeof(MessageID));

	// Handle each packet type individually.
	switch (packetType)
	{
	case ID_REMOTE_NEW_INCOMING_CONNECTION: // A player has connected.
	case ID_NEW_INCOMING_CONNECTION:
		// Player reconnected?
		//OnPlayerConnect(packet);
		break;
	case ID_REMOTE_DISCONNECTION_NOTIFICATION:
	case ID_REMOTE_CONNECTION_LOST:
	case ID_DISCONNECTION_NOTIFICATION:
	case ID_CONNECTION_LOST:
	case PacketType::DISCONNECTED:
		OnClientDisconnect(client);
		break;
	case PacketType::INPUT:
		ReceiveInputPacket(client, inStream);
		break;
	default:
		// Notify the event handler.
		if (packetType >= PacketType::EVENTS_BEGIN && packetType < PacketType::EVENTS_END)
			m_eventHandler->OnClientEvent(client, packetType, inStream);
		break;
	}
}

void NetworkManagerServer::ReceiveInputPacket(ClientProxy* client, RakNet::BitStream& inputPacket)
{
	int moveCount;
	Move move;

	inputPacket.Read(moveCount);
	for (int i = 0; i < moveCount; ++i)
	{
		move.Read(inputPacket);
		client->GetUnprocessedMoveList().AddMove(move);
	}
}

ClientProxy* NetworkManagerServer::GetClientProxy(int playerId)
{
	auto it = m_playerIdToClientProxyMap.find(playerId);
	if (it != m_playerIdToClientProxyMap.end())
		return it->second;
	return nullptr;
}

NetworkManagerServer::PlayerIDToClientProxyMap::iterator NetworkManagerServer::clients_begin()
{
	return m_playerIdToClientProxyMap.begin();
}

NetworkManagerServer::PlayerIDToClientProxyMap::iterator NetworkManagerServer::clients_end()
{
	return m_playerIdToClientProxyMap.end();
}


void NetworkManagerServer::Broadcast(const RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, const ClientProxy* ignoreClient)
{
	RakNet::AddressOrGUID systemAddress = (ignoreClient ? ignoreClient->GetSocketAddress() : RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	m_peerInterface->Send(bitStream, priority, reliability, 0, systemAddress, true);
}

void NetworkManagerServer::Send(const RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, const ClientProxy* client)
{
	m_peerInterface->Send(bitStream, priority, reliability, 0, client->GetSocketAddress(), false);
}
