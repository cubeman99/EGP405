#include "NetworkManagerClient.h"
#include "Messages.h"


NetworkManagerClient::NetworkManagerClient() :
	m_inputManager(NULL),
	m_peerInterface(NULL)
{
}

void NetworkManagerClient::Initialize(RakNet::RakPeerInterface* peerInterface, InputManager* inputManager, const RakNet::SystemAddress& serverAddress)
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
		inputPacket.Write(moveCount - startIndex);//, 2);
		for (int i = startIndex; i < moveCount; ++i)
			moveList[i].Write(inputPacket);
		
		m_peerInterface->Send(&inputPacket, HIGH_PRIORITY,
			RELIABLE_ORDERED, 0, m_serverAddress, false);

		moveList.Clear();
	}
}
