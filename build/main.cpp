
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>

#include "game.h"

#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID

#define MAX_CLIENTS 10     // Max number of players.
#define SERVER_PORT 60000

using namespace std;
using namespace RakNet;


//-----------------------------------------------------------------------------
// GameMessages enum
//-----------------------------------------------------------------------------
enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,

	ID_MENU_OPTION, // Client sends a chosen option ID to the server.
	ID_MENU_DISPLAY_TEXT, // Server sends text to client to display a menu.
	ID_MENU_QUIT,
};


//-----------------------------------------------------------------------------
// RunClient
//-----------------------------------------------------------------------------
void RunClient()
{
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	Packet *packet;

	// Setup the socket.
	SocketDescriptor sd;
	peer->Startup(1, &sd, 1);

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
	
	bool done = false;

	// Receive incoming packets.
	while (!done)
	{
		for (packet = peer->Receive(); packet != NULL && !done;
			peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				done = true;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("We have been disconnected.\n");
				done = true;
				break;
			case ID_CONNECTION_LOST:
				printf("Connection lost.\n");
				done = true;
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted!\n\n");
				break;
			case ID_MENU_DISPLAY_TEXT:
			{
				// We have recieved a new menu!
				RakString text;
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(text);

				// Display the menu text.
				printf("%s\n> ", text.C_String());

				// Wait for user input.
				std::string input;
				getline(std::cin, input);
				printf("\n");

				// Send user input to the server.
				BitStream bsOut;
				bsOut.Write((MessageID) ID_MENU_OPTION);
				bsOut.Write(input.c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					packet->systemAddress, false);
				
				break;
			}
			case ID_MENU_QUIT:
			{
				printf("Quitting the game...\n");
				done = true;
				break;
			}
			default:
				printf("Message with identifier %i has arrived.\n",
					packet->data[0]);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// RunServer
//-----------------------------------------------------------------------------
void RunServer()
{
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	Packet *packet;

	// Initialize the game context.
	GameData game;
	game.m_bankGold = 80;

	// Setup the server socket.
	printf("Starting the server.\n");
	SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	peer->SetMaximumIncomingConnections(MAX_CLIENTS); // Max number of players.
	
	bool done = false;

	// Receive incoming packets.
	while (!done)
	{
		for (packet = peer->Receive(); packet != NULL && !done;
			peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				printf("Another client has disconnected.\n");
				break;
			}
			case ID_REMOTE_CONNECTION_LOST:
			{
				printf("Another client has lost the connection.\n");
				break;
			}
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				printf("The server is full.\n");
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				printf("A client has disconnected.\n");
				break;
			}
			case ID_CONNECTION_LOST:
			{
				printf("A client lost the connection.\n");
				break;
			}
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // A player has connected.
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("Player %s has connected.\n", packet->guid.ToString());

				// Setup the user who just connected.
				PlayerInfo* player = game.ConnectPlayer(packet->guid);

				// Send the first menu text to the client..
				std::ostringstream output;
				output << "Welcome to the game!" << endl << endl;
				output << player->m_menuSystem.GetCurrentMenu()->
					GetMenuText().c_str();

				std::string outputText = player->m_outputStream.str();
				BitStream bsOut;
				bsOut.Write((MessageID)ID_MENU_DISPLAY_TEXT);
				bsOut.Write(output.str().c_str());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					packet->systemAddress, false);

				break;
			}
			case ID_MENU_OPTION: // A player has chosen a menu option.
			{
				PlayerInfo* player = game.GetPlayerInfo(packet->guid);

				// Has the player already exited?
				if (player->m_menuSystem.GetCurrentMenu() == NULL)
					break;

				// Get the input text from the packet.
				RakString input;
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(input);

				// Parse the input text into an option number.
				char* endptr;
				int option = strtol(input.C_String(), &endptr, 10);
				if (*endptr != '\0')
				{
					option = -1; // unable to convert input to integer.
				}

				cout << "Player " << packet->guid.ToString()
					<< " has sent the input (" << option << "): "
					<< input.C_String() << endl;

				// Perform menu code for the given option.
				player->m_menuSystem.GetCurrentMenu()->OnChoose(option, &game,
					player);

				// Check if we exited all menus.
				if (player->m_menuSystem.GetCurrentMenu() == NULL)
				{
					// Send a message for the cient to exit.
					BitStream bsOut;
					bsOut.Write((MessageID) ID_MENU_QUIT);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						packet->systemAddress, false);

					printf("Player %s has quit the game.\n",
						packet->guid.ToString());
					break;
				}
				else 
				{
					// Output the next menu.
					player->m_outputStream << endl <<
						player->m_menuSystem.GetCurrentMenu()->GetMenuText();

					// Send the output text to the player's client.
					BitStream bsOut;
					bsOut.Write((MessageID) ID_MENU_DISPLAY_TEXT);
					bsOut.Write(player->m_outputStream.str().c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						packet->systemAddress, false);
					player->m_outputStream.str(""); // clear the output text.
					player->m_outputStream.clear();
				}

				break;
			}
			default:
				printf("Unknown message with identifier %i has arrived.\n",
					packet->data[0]);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	RakPeerInterface *peer = RakPeerInterface::GetInstance();

	char str[512];
	printf("(C) or (S)erver?\n");
	gets_s(str, sizeof(str));

	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		RunClient();
	}
	else
	{
		RunServer();
	}
	
	RakPeerInterface::DestroyInstance(peer);

	return 0;
}