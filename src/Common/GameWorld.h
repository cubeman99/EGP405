#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <math/Vector2f.h>
#include <graphics/Color.h>
#include <map>
#include "Ball.h"
#include "Config.h"
#include "Slime.h"
#include "Team.h"


typedef int PlayerID;


class GameWorld
{
public:
	typedef std::map<PlayerID, Slime*> PlayerMap;

	enum State
	{
		STATE_WAITING_FOR_PLAYERS,
		STATE_WAITING_FOR_SERVE,
		STATE_GAMEPLAY,
	};

public:
	GameWorld();
	~GameWorld();

	Slime* GetPlayer(PlayerID playerId);
	inline Ball& GetBall() { return m_ball; }
	inline State& GetState() { return m_state; }
	inline Team& GetTeam(int teamIndex) { return m_teams[teamIndex]; }
	inline GameConfig& GetConfig() { return m_config; }

	inline void SetState(State state) { m_state = state; }
	Slime* CreatePlayer(PlayerID playerId);
	void RemovePlayer(PlayerID playerId);

	void SimulatePlayerMovement(Slime* player, float deltaTime);
	void SimulateBallMovement(float deltaTime);

	void OnTeamScore(int teamIndex);

private:
	PlayerMap	m_players;
	State		m_state;
	Ball		m_ball;
	Team		m_teams[2];
	GameConfig	m_config;
};


#endif // _GAME_WORLD_H_