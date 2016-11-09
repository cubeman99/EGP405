#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <math/Vector2f.h>
#include "InputState.h"
#include <vector>


class Move
{
public:
	Move() {}
	Move(const InputState& inputState, float timeStamp, float deltaTime) :
		m_inputState(inputState),
		m_timeStamp(timeStamp),
		m_deltaTime(deltaTime)
	{}

	const InputState& GetInputState() const {return m_inputState;}
	float GetTimestamp() const { return m_timeStamp; }
	float GetDeltaTime() const { return m_deltaTime; }

	void Write(RakNet::BitStream& outStream) const;
	void Read(RakNet::BitStream& inStream);

private:
	InputState m_inputState;
	float m_timeStamp;
	float m_deltaTime;
};


class MoveList
{
public:
	MoveList();

	const Move& AddMove(const InputState& inputState, float timeStamp);

private:
	std::vector<Move> m_moves;
	float m_lastMoveTimeStamp;
};


class InputManager
{
public:
	InputManager();

private:
};


#endif // _INPUT_MANAGER_H_