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
#include <math/Polygonf.h>
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

	int Initialize();
	int Run();
	void Tick(float timeDelta);

private:
	void BeginWaitingForPlayers();
	void BeginNewRound();
	void UpdateBall();
	void OnTeamScore(int teamIndex);

	void ReceivePackets();
	void OnPlayerConnect(RakNet::Packet* connectionPacket);
	void OnPlayerDisconnect(RakNet::Packet* disconnectionPacket);
	void OnPlayerJoinGame(RakNet::Packet* joinPacket);

	bool AreBothTeamsReady();
	int GetNewPlayerId();

	typedef std::map<int, Slime*> PlayerMap;
	typedef std::map<RakNet::RakNetGUID, int> GuidToPlayerMap;

	RakNet::RakPeerInterface* m_peerInterface;

	enum
	{
		STATE_WAIT_FOR_PLAYERS = 0,
		STATE_WAIT_FOR_SERVE,
		STATE_PLAY_GAME,
	};

	GameConfig m_gameConfig;

	int m_state;
	Ball m_ball;
	PlayerMap m_players;
	GuidToPlayerMap m_guidToPlayerMap;
	int m_playerIdCounter;
	float m_serveDelayTimer;
	int m_servingTeamIndex;
	Team m_teams[2];
};


#endif // _SERVER_H_