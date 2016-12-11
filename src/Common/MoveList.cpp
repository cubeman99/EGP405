#include "MoveList.h"


MoveList::MoveList()
{
}

const Move& MoveList::AddMove(const InputState& inputState, int moveNumber, float timeStamp)
{
	float deltaTime = (m_lastMoveTimeStamp >= 0.0f ? timeStamp - m_lastMoveTimeStamp : 0.0f);
	m_moves.push_back(Move(inputState, moveNumber, timeStamp, deltaTime));
	m_lastMoveTimeStamp = timeStamp;
	m_lastMoveNumber = moveNumber;
	return m_moves.back();
}

const Move& MoveList::AddMove(const Move& move)
{
	m_moves.push_back(move);
	m_lastMoveTimeStamp = move.GetTimeStamp();
	m_lastMoveNumber = move.GetMoveNumber();
	return m_moves.back();
}

void MoveList::AddMoves(const MoveList& moveList)
{
	for (unsigned int i = 0; i < moveList.m_moves.size(); i++)
		AddMove(moveList[i]);
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

Move MoveList::RemoveMove(int index)
{
	Move move = m_moves[index];
	m_moves.erase(m_moves.begin() + index);
	return move;
}

