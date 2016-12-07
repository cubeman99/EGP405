#ifndef _MOVE_H_
#define _MOVE_H_

#include "InputState.h"


class Move
{
public:
	Move() {}
	Move(const InputState& inputState, int moveNumber, float timeStamp, float deltaTime) :
		m_inputState(inputState),
		m_moveNumber(moveNumber),
		m_timeStamp(timeStamp),
		m_deltaTime(deltaTime)
	{}

	const InputState& GetInputState() const {return m_inputState;}
	float GetTimeStamp() const { return m_timeStamp; }
	float GetDeltaTime() const { return m_deltaTime; }
	int GetMoveNumber() const { return m_moveNumber; }

	void Write(RakNet::BitStream& outStream) const;
	void Read(RakNet::BitStream& inStream);

private:
	InputState m_inputState;
	float m_timeStamp;
	float m_deltaTime;
	int m_moveNumber;
};


#endif // _MOVE_H_