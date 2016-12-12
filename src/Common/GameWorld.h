#ifndef _GAME_WORLD_H_
#define _GAME_WORLD_H_

#include <math/Vector2f.h>
#include <graphics/Color.h>
#include <map>
#include "Ball.h"
#include "GameConfig.h"
#include "Slime.h"
#include "Team.h"
#include "InputState.h"

typedef int PlayerID;


// Holds game state information and functions for game simulation.
class GameWorld
{
public:
	typedef std::map<PlayerID, Slime*> PlayerMap;

	enum State
	{
		STATE_WAITING_FOR_PLAYERS = 0,
		STATE_WAITING_FOR_SERVE,
		STATE_GAMEPLAY,
	};

public:
	GameWorld();
	~GameWorld();

	void Initialize(const GameConfig& config);

	Slime* GetPlayer(PlayerID playerId);
	inline Ball& GetBall() { return m_ball; }
	inline State& GetState() { return m_state; }
	inline Team& GetTeam(int teamIndex) { return m_teams[teamIndex]; }
	inline GameConfig& GetConfig() { return m_config; }

	inline PlayerMap::iterator players_begin() { return m_players.begin(); }
	inline PlayerMap::iterator players_end() { return m_players.end(); }

	inline void SetState(State state) { m_state = state; }
	Slime* CreatePlayer(PlayerID playerId);
	void RemovePlayer(PlayerID playerId);
	
	void ProcessPlayerInput(Slime* player, const InputState& currentState, float timeDelta);
	void SimulatePlayerMovement(Slime* player, float timeDelta);
	void SimulateBallMovement(float timeDelta);
	void OnTeamScore(int teamIndex);

	void PositionBallAboveNet();
	void PositionBallForServe(int servingTeamIndex);
	void PositionPlayersForServe();

private:
	PlayerMap	m_players;
	State		m_state;
	Ball		m_ball;
	Team		m_teams[2];
	GameConfig	m_config;
};


#endif // _GAME_WORLD_H_
