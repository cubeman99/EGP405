#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <input/Keyboard.h>
#include "MoveList.h"


// Responsible for polling input, and keeping track of moves.
class InputManager
{
public:
	InputManager();

	void Initialize(Keyboard* keyboard);

	void UpdateTimeStamp(float timeDelta);
	void Update(float timeDelta);

	MoveList& GetMoveList() { return m_moveList; }
	const MoveList& GetMoveList() const { return m_moveList; }
	MoveList& GetUnreconciledMoveList() { return m_unreconciledMoves; }
	const MoveList& GetUnreconciledMoveList() const { return m_unreconciledMoves; }
	const InputState& GetInputState() const { return m_inputState; }

	inline float GetTimeStamp() { return m_timeStamp; }

private:
	InputState m_inputState;
	Keyboard* m_keyboard;
	float m_timeStamp;
	int m_moveNumberCounter;

	MoveList m_moveList; // List of unsent moves.
	MoveList m_unreconciledMoves; // List of unreconciled moves (not confirmed by server).
};


#endif // _INPUT_MANAGER_H_