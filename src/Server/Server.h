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
#include "Team.h"
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
	bool AreBothTeamsReady();

	typedef std::map<int, Slime*> PlayerMap;
	typedef std::map<RakNet::RakNetGUID, int> GuidToPlayerMap;

	RakNet::RakPeerInterface* m_peerInterface;
	RakNet::SocketDescriptor m_socketDescriptor;

	enum
	{
		STATE_WAIT_FOR_PLAYERS = 0,
		STATE_WAIT_FOR_SERVE,
		STATE_PLAY_GAME,
	};

	int m_state;
	Config m_config;
	Ball m_ball;
	PlayerMap m_players;
	GuidToPlayerMap m_guidToPlayerMap;
	int m_playerIdCounter;
	float m_serveDelayTimer;
	int m_servingTeamIndex;
	Team m_teams[2];
};


#endif // _SERVER_H_