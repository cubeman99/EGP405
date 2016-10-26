
#include <iostream>
#include <stdio.h>
#include <string>
#include <RakNet/RakPeerInterface.h>
#include "Server.h"


#define MAX_CLIENTS 20
#define SERVER_PORT 60000


int main(int argc, char** argv)
{
	int returnCondition;

	Server server;

	returnCondition = server.Initialize();
	if (returnCondition == 0)
		returnCondition = server.Run();
	
	RakNet::RakPeerInterface::DestroyInstance(RakNet::RakPeerInterface::GetInstance());

	if (returnCondition != 0)
		system("pause");
	return returnCondition;
}

