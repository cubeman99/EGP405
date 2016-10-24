#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <map>
#include "Ball.h"
#include "Slime.h"
#include "Messages.h"
#include "Config.h"


struct PlayerInfo
{
	RakNet::RakNetGUID guid;

};


class Server
{
public:
	Server();
	~Server();

	int Run();
	void Tick(float timeDelta);

private:
	void ReceivePackets();
	void UpdateBall();

	typedef std::map<int, Slime*> PlayerMap;
	typedef std::map<RakNet::RakNetGUID, int> GuidToPlayerMap;

	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SocketDescriptor m_socketDescriptor;

	Config m_config;
	Ball m_ball;
	PlayerMap m_players;
	GuidToPlayerMap m_guidToPlayerMap;
	int m_playerIdCounter;

};


#endif // _SERVER_H_