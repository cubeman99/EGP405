#include "InputState.h"


InputState::InputState() :
	m_desiredRightAmount(0),
	m_desiredLeftAmount(0),
	m_isJumping(false)
{
}

void InputState::Reset()
{
	m_desiredRightAmount = 0;
	m_desiredLeftAmount = 0;
	m_isJumping = false;
}

void InputState::Write(RakNet::BitStream& outStream) const
{
	outStream.Write(m_desiredRightAmount);
	outStream.Write(m_desiredLeftAmount);
	outStream.Write(m_isJumping);
}

void InputState::Read(RakNet::BitStream& inStream)
{
	inStream.Read(m_desiredRightAmount);
	inStream.Read(m_desiredLeftAmount);
	inStream.Read(m_isJumping);
}
