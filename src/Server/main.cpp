
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include "Server.h"


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


int main(int argc, char** argv)
{
	Server server;
	int rc = server.Run();
	
	RakNet::RakPeerInterface::DestroyInstance(RakNet::RakPeerInterface::GetInstance());

	if (rc != 0)
		system("pause");
	return 0;
}

