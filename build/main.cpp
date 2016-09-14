
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

using namespace std;
using namespace RakNet;

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,

	ID_MENU_OPTION, // Client sends a chosen option ID to the server.
	ID_MENU_DISPLAY_TEXT, // Server sends text to client to display a menu.
};

//
//
//class Menu
//{
//public:
//
//	Menu();
//
//	void AddOption(const std::string& option);
//
//private:
//
//	std::vector<std::string> m_options;
//
//};


//Menu::Menu();


void RunClient()
{
	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet *packet;

	// Prompt for the IP address to connect to
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
	bool anyErrors = false;

	// 1. Receive a menu text.
	// 2. Wait for user input for chosen option.

	while (!done)
	{
		// Receive incoming packets.
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("We have been disconnected.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_CONNECTION_LOST:
				printf("Connection lost.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted.\n");
				done = true;
				break;
			}
		}
	}

	// Wait for a menu text message.
	done = false;
	while (!done)
	{
		// Receive incoming packets.
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{

			switch (packet->data[0])
			{
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("We have been disconnected.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_CONNECTION_LOST:
				printf("Connection lost.\n");
				anyErrors = true;
				done = true;
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted.\n");
				done = true;
				break;
			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
				break;
			}
			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}
}

void RunServer()
{
	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet *packet;

	printf("Starting the server.\n");
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

}


int main(void)
{
	char str[512];

	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	RakNet::Packet *packet;

	printf("(C) or (S)erver?\n");
	gets_s(str, sizeof(str));

	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		RakNet::SocketDescriptor sd;
		peer->Startup(1, &sd, 1);

		RunServer();
	}
	else {
		RakNet::SocketDescriptor sd(SERVER_PORT, 0);
		peer->Startup(MAX_CLIENTS, &sd, 1);

		RunClient();
	}

	if (isServer)
	{
		printf("Starting the server.\n");
		// We need to let the server accept incoming connections from the clients
		peer->SetMaximumIncomingConnections(MAX_CLIENTS);
	}
	else {
		printf("Enter server IP or hit enter for 127.0.0.1\n");
		gets_s(str, sizeof(str));
		if (str[0] == 0) {
			strcpy_s(str, "127.0.0.1");
		}
		printf("Starting the client.\n");
		peer->Connect(str, SERVER_PORT, 0, 0);

	}

	bool done = false;

	while (!done)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");

				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID) ID_GAME_MESSAGE_1);
				bsOut.Write("Hello world");
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

				// Send some input text to the server.
				if (!isServer)
				{
					while (true)
					{
						string input;

						//cin >> input;
						getline(cin, input);

						if (input[0] == 'q')
						{
							break;
							done = true;
							cout << "Shutting down client..." << endl;
						}

						RakNet::BitStream bsOut2;
						bsOut2.Write((RakNet::MessageID) ID_GAME_MESSAGE_1);
						bsOut2.Write(input.c_str());
						peer->Send(&bsOut2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
					}
				}

			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
			{
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(rs);
				printf("%s\n", rs.C_String());
			}
			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}
	
	RakNet::RakPeerInterface::DestroyInstance(peer);

	return 0;
}