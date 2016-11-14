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

	void UpdateBall();
	void ProcessPlayerInput(Slime* player, const InputState& currentState, float deltaTime);
	void SimulatePlayerMovement(Slime* player, float deltaTime);

	void BeginWaitingForPlayers();
	void BeginNewRound();
	void OnTeamScore(int teamIndex);

	bool AreBothTeamsReady();

	enum
	{
		STATE_WAIT_FOR_PLAYERS = 0,
		STATE_WAIT_FOR_SERVE,
		STATE_PLAY_GAME,
	};

	typedef std::map<int, Slime*> PlayerMap;

	// Game config
	GameConfig m_gameConfig;

	// Game state
	float m_serveDelayTimer;
	int m_servingTeamIndex;
	int m_state;
	Ball m_ball;
	PlayerMap m_players;
	Team m_teams[2];

	// Managers
	NetworkManagerServer m_networkManager;
};


#endif // _SERVER_H_