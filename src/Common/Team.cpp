#include "Team.h"


Team::Team()
{
}

Team::Team(int teamIndex, const std::string& name, const Rect2f& playRegion) :
	m_teamIndex(teamIndex),
	m_name(name),
	m_playRegion(playRegion),
	m_score(0)
{
}


void Team::SetTeamIndex(int teamIndex)
{
	m_teamIndex = teamIndex;
}

void Team::SetScore(int score)
{
	m_score = score;
}

void Team::SetName(const std::string& name)
{
	m_name = name;
}

void Team::SetPlayRegion(const Rect2f& playRegion)
{
	m_playRegion = playRegion;
}


int Team::GetTeamIndex() const
{
	return m_teamIndex;
}

int Team::GetScore() const
{
	return m_score;
}

const std::string& Team::GetName() const
{
	return m_name;
}

const Rect2f& Team::GetPlayRegion() const
{
	return m_playRegion;
}
