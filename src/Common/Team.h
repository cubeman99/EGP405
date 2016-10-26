#ifndef _TEAM_H_
#define _TEAM_H_

#include <math/Vector2f.h>
#include <math/Rect2f.h>
#include "Config.h"
#include <string>


class Team
{
public:
	Team();
	Team(int teamIndex, const std::string& name, const Rect2f& playRegion, const GameConfig& config);

	void SetTeamIndex(int teamIndex);
	void SetScore(int score);
	void SetName(const std::string& name);
	void SetPlayRegion(const Rect2f& playRegion);

	int GetTeamIndex() const;
	int GetScore() const;
	const std::string& GetName() const;
	const Rect2f& GetPlayRegion() const;

	inline const Vector2f& GetBallServePosition() const { return m_ballServePosition; }
	inline const Vector2f& GetPlayerSpawnPosition() const { return m_playerSpawnPosition; }

private:
	int			m_teamIndex;
	std::string	m_name;
	int			m_score;
	Rect2f		m_playRegion;
	Vector2f	m_ballServePosition;
	Vector2f	m_playerSpawnPosition;
};


#endif // _TEAM_H_