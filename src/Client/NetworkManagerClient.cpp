#include "NetworkManagerClient.h"
#include "Messages.h"


NetworkManagerClient::NetworkManagerClient() :
	m_inputManager(NULL),
	m_peerInterface(NULL)
{
}

NetworkManagerClient::~NetworkManagerClient()
{
	for (auto it = m_playerIdToPlayerProxyMap.begin();
		it != m_playerIdToPlayerProxyMap.end(); it++)
	{
		delete it->second;
	}
	m_playerIdToPlayerProxyMap.clear();
}

void NetworkManagerClient::Initialize(RakNet::RakPeerInterface* peerInterface,
	InputManager* inputManager, const RakNet::SystemAddress& serverAddress)
{
	m_peerInterface = peerInterface;
	m_inputManager = inputManager;
	m_serverAddress = serverAddress;
}

void NetworkManagerClient::SendInputPacket()
{
	// Only send if there's any input to send!
	MoveList& moveList = m_inputManager->GetMoveList();

	if (moveList.HasMoves())
	{
		RakNet::BitStream inputPacket;
		inputPacket.Write((RakNet::MessageID) PacketType::INPUT);

		// we only want to send the last three moves
		int moveCount = moveList.GetMoveCount();
		//int startIndex = (moveCount > 3 ? moveCount - 3 - 1 : 0);
		int startIndex = 0;
		inputPacket.Write(moveCount - startIndex);
		for (int i = startIndex; i < moveCount; ++i)
			moveList[i].Write(inputPacket);
		
		Send(&inputPacket, HIGH_PRIORITY, RELIABLE_ORDERED);

		moveList.Clear();
	}
}

void NetworkManagerClient::Send(const RakNet::BitStream* bitStream,
	PacketPriority priority, PacketReliability reliability)
{
	m_peerInterface->Send(bitStream, priority, reliability, 0, m_serverAddress, false);
}

void NetworkManagerClient::CloseConnection()
{
	m_peerInterface->CloseConnection(m_serverAddress, true, 0, IMMEDIATE_PRIORITY);
	Sleep(100);
}


void NetworkManagerClient::AddPlayerProxy(PlayerID playerId)
{
	if (GetPlayerProxy(playerId) == nullptr)
		m_playerIdToPlayerProxyMap[playerId] = new PlayerProxy(playerId);
}
void NetworkManagerClient::RemovePlayerProxy(PlayerID playerId)
{
	if (GetPlayerProxy(playerId) != nullptr)
		m_playerIdToPlayerProxyMap.erase(playerId);
}

PlayerProxy* NetworkManagerClient::GetPlayerProxy(PlayerID playerId)
{
	auto it = m_playerIdToPlayerProxyMap.find(playerId);
	if (it != m_playerIdToPlayerProxyMap.end())
		return it->second;
	return nullptr;
}

void NetworkManagerClient::UpdatePlayerProxies(float timeDelta)
{
	for (auto it = m_playerIdToPlayerProxyMap.begin();
		it != m_playerIdToPlayerProxyMap.end(); it++)
	{
		it->second->UpdateInterpolation(timeDelta);
	}
}

