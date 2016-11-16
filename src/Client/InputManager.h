#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <math/Vector2f.h>
#include <input/Keyboard.h>
#include <input/Mouse.h>
#include "InputState.h"
#include "Move.h"
#include "MoveList.h"
#include <vector>



class InputManager
{
public:
	InputManager();

	void Initialize(Keyboard* keyboard);

	void Update(float timeDelta);

	MoveList& GetMoveList() { return m_moveList; }
	const MoveList& GetMoveList() const { return m_moveList; }

private:
	InputState m_inputState;
	MoveList m_moveList;
	Keyboard* m_keyboard;
	float m_timeStamp;
};


#endif // _INPUT_MANAGER_H_