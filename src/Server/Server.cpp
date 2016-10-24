#include "Server.h"

using namespace RakNet;


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


Server::Server() :
	m_socketDescriptor(SERVER_PORT, 0)
{
	m_peerInterface = RakPeerInterface::GetInstance();

	// Setup the server socket.
	printf("Starting the server.\n");
	SocketDescriptor sd(SERVER_PORT, 0);
	m_peerInterface->Startup(MAX_CLIENTS, &m_socketDescriptor, 1);
	m_peerInterface->SetMaximumIncomingConnections(MAX_CLIENTS); // Max number of players.

	
}