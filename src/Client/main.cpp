//-----------------------------------------------------------------------------
//
// Assignment 2 - Networked game of tic-tac-toe
//
// Authors:
//  * David Jordan
//  * Garret Moran
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>

#include "board.h"

#include <SDL2/SDL.h>
#undef main

#include <GameLib/Application.h>
#include <GameLib/graphics/Graphics.h>
#include <Common/Config.h>
#include "ClientApp.h"

#define MAX_CLIENTS 1     // This is a 2 player game.
#define SERVER_PORT 60000

using namespace std;
using namespace RakNet;


//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------

int main()
{	
	RakPeerInterface *peer = RakPeerInterface::GetInstance();

	ClientApp app(peer);

	// Setup the client socket.
	SocketDescriptor sd;
	peer->Startup(1, &sd, 1);
	peer->SetOccasionalPing(true);

	// Prompt for the IP address to connect to.
	char str[512];
	printf("Enter server IP or hit enter for 127.0.0.1\n");
	gets_s(str, sizeof(str));
	if (str[0] == 0)
	{
		strcpy_s(str, "127.0.0.1");
	}

	// Connect to the server.
	printf("Starting the client.\n");
	peer->Connect(str, SERVER_PORT, 0, 0);

	RakNet::SystemAddress serverAddress;
	int connectionResult = 0;

	// Wait for connection.
	Packet *packet;
	while (connectionResult == 0 || connectionResult == 1)
	{
		for (packet = peer->Receive(); packet != NULL && connectionResult != 2;
			peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			// CLIENT SPECIFIC MESSAGES:
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				printf("Server is full!\n");
				connectionResult = -1;
				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Connected successfully!\n");
				serverAddress = packet->systemAddress;
				connectionResult = 1;
				break;
			}
			case GameMessages::ACCEPT_CONNECTION:
			{
				app.ReadConnectionAcceptedPacket(packet);
				connectionResult = 2;
				break;
			}
			}
		}
	}

	// Run the client application.
	if (connectionResult == 2)
	{
		app.Initialize("Slime Volleyball",
			(int)app.GetConfig().VIEW_WIDTH,
			(int) app.GetConfig().VIEW_HEIGHT);
		app.Run();
	}
	else
	{
		system("pause");
	}

	RakPeerInterface::DestroyInstance(peer);
	return 0;
}
