#ifndef _SERVER_H_
#define _SERVER_H_

#include <Windows.h>


#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>


struct PlayerInfo
{
	RakNet::RakNetGUID guid;

};


class Server
{
public:
	Server();

private:

	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SocketDescriptor m_socketDescriptor;
};


#endif // _SERVER_H_