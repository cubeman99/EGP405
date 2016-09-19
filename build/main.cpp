
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>

#include "game.h"
#include "keyboard.h"
#include "menu_display.h"

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

	ID_MENU_OPTION,             // Client sends a chosen option ID to the server.
	ID_MENU_DISPLAY_TEXT,       // Server sends text to client to display a menu.
	ID_MENU_QUIT,

	ID_MENU_UPDATE_DESCRIPTION, // Only the menu description has changed.
	ID_MENU_NEW_MENU,           // A new menu has been entered.
};


//-----------------------------------------------------------------------------
// RunServer
//-----------------------------------------------------------------------------
void RunServer()
{
	srand((unsigned int) time(0));

	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	Packet *packet;

	// Initialize the game context.
	GameData game;

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
				Menu* currentMenu = player->m_menuSystem.GetCurrentMenu();

				// Send a new-menu message.
				BitStream bsOut;
				bsOut.Write((MessageID)ID_MENU_NEW_MENU);
				bsOut.Write(currentMenu->GetTitle().c_str());
				bsOut.Write(currentMenu->GetDescription().c_str());
				bsOut.Write(currentMenu->GetOptionCount());
				for (int i = 0; i < currentMenu->GetOptionCount(); i++)
					bsOut.Write(currentMenu->GetOption(i).c_str());
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
				int option = 0;
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(option);
				cout << "Player " << packet->guid.ToString()
					 << " has chosen option " << option << endl;

				// Perform menu code for the given option.
				Menu* previousMenu = player->m_menuSystem.GetCurrentMenu();
				player->m_menuSystem.GetCurrentMenu()->OnChoose(option, &game, player);
				Menu* currentMenu = player->m_menuSystem.GetCurrentMenu();

				// Check if we exited all menus or changed menus.
				if (currentMenu == NULL)
				{
					// Send a message for the cient to exit.
					BitStream bsOut;
					bsOut.Write((MessageID) ID_MENU_QUIT);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						packet->systemAddress, false);
					cout << "Player " << packet->guid.ToString()
						 << " has quit the game" << endl;
					break;
				}
				else if (currentMenu != previousMenu)
				{
					// Send a new-menu message.
					BitStream bsOut;
					bsOut.Write((MessageID) ID_MENU_NEW_MENU);
					bsOut.Write(currentMenu->GetTitle().c_str());
					bsOut.Write(currentMenu->GetDescription().c_str());
					bsOut.Write(currentMenu->GetOptionCount());
					for (int i = 0; i < currentMenu->GetOptionCount(); i++)
						bsOut.Write(currentMenu->GetOption(i).c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						packet->systemAddress, false);
					cout << "Sending a new-menu message to player "
						 << packet->guid.ToString() << endl;
				}
				else
				{
					// Send a update description message.
					BitStream bsOut;
					bsOut.Write((MessageID) ID_MENU_UPDATE_DESCRIPTION);
					bsOut.Write(currentMenu->GetDescription().c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
						packet->systemAddress, false);

					cout << "Sending a udpate-description message to player "
						 << packet->guid.ToString() << endl;
				}

				break;
			}
			default:
				printf("Unknown message with identifier %i has arrived.\n",
					packet->data[0]);
			}
		}
	}

	Keyboard::terminate();
}


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
	
	// Client game initialization.
	MenuDisplay menuDisplay;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	Keyboard::initialize();
	SystemAddress serverAddress;
	bool done = false;
	bool isConnected = false;

	// Hide the text cursor.
	CONSOLE_CURSOR_INFO cursorInfo;
	if (GetConsoleCursorInfo(handle, &cursorInfo) != 0)
	{
		cursorInfo.bVisible = (BOOL)false;
		SetConsoleCursorInfo(handle, &cursorInfo);
	}
	
	// Create a menu while waiting to connect.
	vector<string> options;
	options.push_back("Quit");
	menuDisplay.NewMenu(
		"Connecting",
		"Waiting for connection...",
		options);
	menuDisplay.Display(handle);

	// Receive incoming packets.
	while (!done)
	{
		Keyboard::update();

		// Menu navigation keys.
		if (isConnected)
		{
			if (Keyboard::KEY_DOWN->pressed())
			{
				menuDisplay.MoveCursorNext();
				menuDisplay.Display(handle);
			}
			if (Keyboard::KEY_UP->pressed())
			{
				menuDisplay.MoveCursorBack();
				menuDisplay.Display(handle);
			}
			if (Keyboard::KEY_ENTER->pressed())
			{
				// Send option index to server.
				BitStream bsOut;
				bsOut.Write((MessageID) ID_MENU_OPTION);
				bsOut.Write(menuDisplay.GetCursorIndex());
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAddress, false);
			}
		}

		// Read all incoming packets.
		for (packet = peer->Receive(); packet != NULL && !done;
			peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			serverAddress = packet->systemAddress;

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
				isConnected = true;
				break;
			case ID_MENU_UPDATE_DESCRIPTION: // The menu's description has changed.
			{
				RakString description;

				// Read the packet.
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(description);
				
				// Update the menu's description.
				menuDisplay.SetDescription(description.C_String());
				menuDisplay.Display(handle);
				break;
			}
			case ID_MENU_NEW_MENU: // We have recieved a new menu!
			{
				RakString title;
				RakString description;
				RakString option;
				vector<string> options;
				int numOptions = 0;

				// Read the packet.
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(title);
				bsIn.Read(description);
				bsIn.Read(numOptions);
				for (int i = 0; i < numOptions; i++)
				{
					bsIn.Read(option);
					options.push_back(option.C_String());
				}

				// Update the menu display.
				menuDisplay.NewMenu(title.C_String(), description.C_String(), options);
				menuDisplay.Display(handle);

				break;
			}
			case ID_MENU_QUIT: // The client has chosen to quit the game.
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

	Keyboard::terminate();
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
