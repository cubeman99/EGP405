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
#include "MoveList.h"
#include "NetworkManagerServer.h"
#include "GameWorld.h"


class Server : public NetworkEventHandler
{
public:
	Server();
	~Server();

	int Initialize();
	int Run();
	void Tick(float timeDelta);

	void OnClientConnect(ClientProxy* client) override;
	void OnClientDisconnect(ClientProxy* client) override;
	void OnClientEvent(ClientProxy* client, NetworkEventID eventId, RakNet::BitStream& inStream) override;

	void OnPlayerJoinTeam(ClientProxy* client, RakNet::BitStream& inStream);

private:
	void BeginWaitingForPlayers();
	void BeginNewRound();
	void OnTeamScore(int teamIndex);
	bool AreBothTeamsReady();

	NetworkManagerServer m_networkManager;
	GameWorld m_gameWorld;
	float m_serveDelayTimer;
	int m_servingTeamIndex;

	float m_timeStamp;
};


#endif // _SERVER_H_