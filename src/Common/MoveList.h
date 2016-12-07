#ifndef _MOVE_LIST_H_
#define _MOVE_LIST_H_

#include "Move.h"
#include <vector>


class MoveList
{
public:
	MoveList();

	const Move& AddMove(const InputState& inputState, int moveNumber, float timeStamp);
	const Move& AddMove(const Move& move);
	bool AddMoveIfNew(const Move& move);
	void Clear();

	inline bool HasMoves() const { return !m_moves.empty(); }
	inline int GetMoveCount() const { return (int) m_moves.size(); }

	inline Move& operator[](int index) { return m_moves[index]; }
	inline const Move& operator[](int index) const { return m_moves[index]; }

	inline float GetLastMoveTimeStamp() const { return m_lastMoveTimeStamp; }
	inline int GetLastMoveNumber() const { return m_lastMoveNumber; }

private:
	std::vector<Move> m_moves;
	float m_lastMoveTimeStamp;
	int m_lastMoveNumber;
};


#endif // _MOVE_LIST_H_