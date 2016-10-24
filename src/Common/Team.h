#ifndef _TEAM_H_
#define _TEAM_H_

#include <math/Vector2f.h>
#include <math/Rect2f.h>
#include <string>


class Team
{
public:
	Team();
	Team(int teamIndex, const std::string& name, const Rect2f& playRegion);

	void SetTeamIndex(int teamIndex);
	void SetScore(int score);
	void SetName(const std::string& name);
	void SetPlayRegion(const Rect2f& playRegion);

	int GetTeamIndex() const;
	int GetScore() const;
	const std::string& GetName() const;
	const Rect2f& GetPlayRegion() const;

private:
	int			m_teamIndex;
	std::string	m_name;
	int			m_score;
	Rect2f		m_playRegion;
};


#endif // _TEAM_H_