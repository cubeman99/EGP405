#include "InputManager.h"


InputManager::InputManager() :
	m_keyboard(NULL)
{
}

void InputManager::Initialize(Keyboard* keyboard)
{
	m_keyboard = keyboard;
	m_timeStamp = 0.0f;
	m_moveNumberCounter = 0;
}

void InputManager::Update(float timeDelta)
{
	m_timeStamp += timeDelta;

	// Left/Right: Move horizontally
	if (m_keyboard->IsKeyDown(Keys::LEFT))
		m_inputState.m_desiredLeftAmount += 1;
	if (m_keyboard->IsKeyDown(Keys::RIGHT))
		m_inputState.m_desiredRightAmount += 1;

	// Up: Jump
	if (m_keyboard->IsKeyDown(Keys::UP))
		m_inputState.m_isJumping = true;

	// Add one move per tick.
	Move move(m_inputState, m_moveNumberCounter, m_timeStamp, timeDelta);
	m_moveList.AddMove(move);
	m_unreconciledMoves.AddMove(move);
	m_moveNumberCounter++;

	m_inputState.Reset();
}

