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
// GameMessages
//-----------------------------------------------------------------------------
enum GameMessages
{
	ID_NEXT_MOVE = ID_USER_PACKET_ENUM + 1, // A player made a move.
};

//-----------------------------------------------------------------------------
// GameData
//-----------------------------------------------------------------------------
struct GameData
{
public:
	bool isMyTurn;
	char mySymbol;
	Board board;
	bool isConnected;
	SystemAddress opponentAddress;

	GameData()
	{
		mySymbol = 'O';
		isMyTurn = false;
		isConnected = false;
	}
};


//-----------------------------------------------------------------------------
// ReceivePackets
//-----------------------------------------------------------------------------
void ReceivePackets(RakPeerInterface* peer, GameData* gameData)
{
	Packet *packet;

	for (packet = peer->Receive(); packet != NULL;
		peer->DeallocatePacket(packet), packet = peer->Receive())
	{
		switch (packet->data[0])
		{
			// SERVER SPECIFIC MESSAGES:
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				printf("Our opponent has disconnected.\n");
				break;
			}
			case ID_REMOTE_CONNECTION_LOST:
			{
				printf("Our opponent has lost the connection.\n");
				break;
			}
			case ID_REMOTE_NEW_INCOMING_CONNECTION: // A player has connected.
			case ID_NEW_INCOMING_CONNECTION:
			{
				printf("Our opponent has connected.\n");

				gameData->opponentAddress = packet->systemAddress;
				gameData->isConnected = true;

				break;
			}

			// CLIENT SPECIFIC MESSAGES:
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				printf("That address is already playing a game!\n");
				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("We have connected to our opponent!\n");
				gameData->opponentAddress = packet->systemAddress;
				gameData->isConnected = true;
				break;
			}

			// SERVER/CLIENT SHARED MESSAGES:
			case ID_DISCONNECTION_NOTIFICATION:
			{
				printf("We have been disconnected.\n");
				gameData->isConnected = false;
				break;
			}
			case ID_CONNECTION_LOST:
			{
				printf("Connection lost.\n");
				gameData->isConnected = false;
				break;
			}
			case ID_NEXT_MOVE:
			{
				char opponentSymbol;
				unsigned char moveIndex;

				// Read the packet and update the board.
				BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(MessageID));
				bsIn.Read(opponentSymbol);
				bsIn.Read(moveIndex);

				gameData->board.SetSpace(moveIndex, opponentSymbol);
				gameData->isMyTurn = true;
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// PlayGame
//-----------------------------------------------------------------------------
void PlayGame(bool isServer)
{
	RakPeerInterface *peer = RakPeerInterface::GetInstance();
	GameData gameData;

	// Server/client setup code.
	if (isServer)
	{
		gameData.mySymbol = 'X';
		gameData.isMyTurn = true;

		// Setup the server socket.
		printf("Starting the server.\n");
		SocketDescriptor sd(SERVER_PORT, 0);
		peer->Startup(MAX_CLIENTS, &sd, 1);
		peer->SetMaximumIncomingConnections(MAX_CLIENTS); // Max number of players.
	}
	else
	{
		gameData.mySymbol = 'O';
		gameData.isMyTurn = false;

		// Setup the client socket.
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
	}

	// Wait for connection.
	while (!gameData.isConnected)
	{
		ReceivePackets(peer, &gameData);
	}
	printf("\n");

	if (!isServer)
	{
		gameData.board.Display();
		printf("\n");
	}

	char winner;
	bool isDraw;
	int input;

	// Game loop.
	while (!gameData.board.CheckWin(winner, isDraw))
	{
		// Wait until the opponent has taken thier turn.
		if (!gameData.isMyTurn)
		{
			printf("Waiting for opponent to make a move... ");
			fflush(stdout);

			while (!gameData.isMyTurn)
			{
				ReceivePackets(peer, &gameData);
			}

			printf("done.\n\n");
		}

		// Display the new board after the opponent made a move.
		gameData.board.Display();
		printf("\n");

		// Check if the game has ended.
		if (gameData.board.CheckWin(winner, isDraw))
			break;

		// Prompt the user to take their turn.
		bool isValidMove = false;

		while (!isValidMove)
		{
			printf("Enter space number for %c: ", gameData.mySymbol);

			std::cin >> input;
			input -= 1; // internal index is zero-based.

			printf("\n");

			// Validate the input.
			if (input < 0 || input >= 9)
			{
				isValidMove = false;
				printf("Invalid space number!\n");
				printf("Please enter a number between 1 and 9.\n\n");
				gameData.board.Display();
				printf("\n");
			}
			else if (!gameData.board.IsSpaceEmpty(input))
			{
				isValidMove = false;
				printf("There is already a '%c' in that space!\n", gameData.board.GetSpace(input));
				printf("Please enter the number of an empty space.\n\n");
				gameData.board.Display();
				printf("\n");
			}
			else
			{
				isValidMove = true;
				gameData.board.SetSpace(input, gameData.mySymbol);

				// Send a next-move message.
				BitStream bsOut;
				bsOut.Write((MessageID) ID_NEXT_MOVE);
				bsOut.Write((char) gameData.mySymbol);
				bsOut.Write((unsigned char) input);
				peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
					gameData.opponentAddress, false);

				gameData.isMyTurn = false;
			}
		}

		// Display the new board after the user made a move.
		gameData.board.Display();
		printf("\n");
	}

	// Display who won or if there was a tie.
	if (isDraw)
		printf("The game ended in a tie!\n\n");
	else if (winner == gameData.mySymbol)
		printf("You won!\n\n");
	else
		printf("You lost, player %c won.\n\n", winner);
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------


void JuliaSet()
{
	int winWidth = 199;
	int winHeight = 120;

	float xMin = -1.2f;
	float xMax =  1.2f;
	float yMin = -1.0f;
	float yMax =  1.0f;


	//xMin = 0.1f;
	//xMax = 0.15f;
	//yMin = 0.4f;
	//yMax = 0.44f;


	float real = -0.4f;
	float imag = 0.6f;

	int maxIterations = 100;

	float cRe = -0.7;
	float cIm = 0.27015;
	cRe = -0.8f;
	cIm = 0.156f;
	cRe = -0.4f;
	cIm = 0.6f;
	cRe = 0.285f;
	cIm = 0.01f;

	/*cRe = -0.835f;
	cIm = -0.2321f;*/

	float newRe, newIm, oldRe, oldIm;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleScreenBufferSize(hConsole, {200, 200});

	//SetConsoleTextAttribute(hConsole, rand() % 26);

	//int colors[4]  = {0, 8, 7, 15};
	int numColors = 7;
	int colors[]  = {8, 7, 7, 7, 7, 15, 15};
	char symbols[] = {255, 255, 176, 177, 178, 178, 219};

	for (int y = 0; y < winHeight; y++)
	{
		for (int x = 0; x < winWidth; x++)
		{
			newRe = xMin + (((float) x / winWidth) * (xMax - xMin));
			newIm = yMax - (((float) y / winHeight) * (yMax - yMin));

			int i;
			for (i = 0; i < maxIterations; i++)
			{
				//remember value of previous iteration
				oldRe = newRe;
				oldIm = newIm;
				//the actual iteration, the real and imaginary part are calculated
				newRe = oldRe * oldRe - oldIm * oldIm + cRe;
				newIm = 2 * oldRe * oldIm + cIm;

				//if the point is outside the circle with radius 2: stop
				if ((newRe * newRe + newIm * newIm) > 4)
					break;
			}

			int p = (i * numColors) / (maxIterations + 1);
			if (p >= maxIterations)
				p = 0;
			SetConsoleTextAttribute(hConsole, colors[p]);

			//printf("%c", (i < maxIterations) ? 'X' : ' ');

			//printf("%c", (i < maxIterations) ? '#' : ' ');
			//printf("%c", symbols[p]);

			if (i < maxIterations)
				SetConsoleTextAttribute(hConsole, 9 + (i % 6));
			else
				SetConsoleTextAttribute(hConsole, 0);
			printf("%c", (char) 219);
		}
		printf("\n");
	}

}



int main()
{
	ClientApp app;

	app.Initialize("Slime Volleyball",
		app.GetConfig().VIEW_WIDTH,
		app.GetConfig().VIEW_HEIGHT);
	app.Run();

	//JuliaSet();
	//system("pause");
	return 0;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < 10000; i++)
	{
		char c = (rand() % 256);

		if (c == '\t' || c == '\n')
			continue;

		SetConsoleTextAttribute(hConsole, rand() % 26);
		printf("%c", c);
	}

	system("pause");
	return 0;

	RakPeerInterface *peer = RakPeerInterface::GetInstance();

	char str[512];
	printf("(C) or (S)erver?\n");
	gets_s(str, sizeof(str));

	if ((str[0] == 'c') || (str[0] == 'C'))
	{
		PlayGame(false); // play game as client.
	}
	else
	{
		PlayGame(true); // play game as server.
	}
	
	RakPeerInterface::DestroyInstance(peer);

	system("pause");
	return 0;
}
