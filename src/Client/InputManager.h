#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <input/Keyboard.h>
#include "MoveList.h"


class InputManager
{
public:
	InputManager();

	void Initialize(Keyboard* keyboard);

	void Update(float timeDelta);

	MoveList& GetMoveList() { return m_moveList; }
	const MoveList& GetMoveList() const { return m_moveList; }
	MoveList& GetUnreconciledMoveList() { return m_moveList; }
	const MoveList& GetUnreconciledMoveList() const { return m_moveList; }

	inline float GetTimeStamp() { return m_timeStamp; }

private:
	InputState m_inputState;
	Keyboard* m_keyboard;
	float m_timeStamp;
	int m_moveNumberCounter;

	MoveList m_moveList; // List of unsent moves.
	MoveList m_unreconciledMoves; // List of unreconciled moves (not corrected by server).
};


#endif // _INPUT_MANAGER_H_