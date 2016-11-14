#include "MoveList.h"


MoveList::MoveList()
{
}

const Move& MoveList::AddMove(const InputState& inputState, float timeStamp)
{
	float deltaTime = (m_lastMoveTimeStamp >= 0.0f ? timeStamp - m_lastMoveTimeStamp : 0.0f);
	m_moves.emplace_back(inputState, timeStamp, deltaTime);
	m_lastMoveTimeStamp = timeStamp;
	return m_moves.back();
}

const Move& MoveList::AddMove(const Move& move)
{
	m_moves.emplace_back(move);
	m_lastMoveTimeStamp = move.GetTimeStamp();
	return m_moves.back();
}

void MoveList::Clear()
{
	m_moves.clear();
}
