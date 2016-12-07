#include "MoveList.h"


MoveList::MoveList()
{
}

const Move& MoveList::AddMove(const InputState& inputState, int moveNumber, float timeStamp)
{
	float deltaTime = (m_lastMoveTimeStamp >= 0.0f ? timeStamp - m_lastMoveTimeStamp : 0.0f);
	m_moves.push_back(Move(inputState, moveNumber, timeStamp, deltaTime));
	m_lastMoveTimeStamp = timeStamp;
	return m_moves.back();
}

const Move& MoveList::AddMove(const Move& move)
{
	m_moves.push_back(move);
	m_lastMoveTimeStamp = move.GetTimeStamp();
	return m_moves.back();
}

bool MoveList::AddMoveIfNew(const Move& move)
{
	float timeStamp = move.GetTimeStamp();

	if (timeStamp > m_lastMoveTimeStamp)
	{
		float timeDelta = (m_lastMoveTimeStamp >= 0.0f ?
			timeStamp - m_lastMoveTimeStamp : 0.0f);
		m_lastMoveTimeStamp = timeStamp;
		m_moves.push_back(Move(move.GetInputState(), move.GetMoveNumber(), timeStamp, timeDelta));
		m_lastMoveNumber = move.GetMoveNumber();
		return true;
	}
	return false;
}

void MoveList::Clear()
{
	m_moves.clear();
}
