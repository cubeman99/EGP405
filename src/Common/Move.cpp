#include "Move.h"


void Move::Write(RakNet::BitStream& outStream) const
{
	outStream.Write(m_timeStamp);
	outStream.Write(m_deltaTime);
	m_inputState.Write(outStream);
}

void Move::Read(RakNet::BitStream& inStream)
{
	inStream.Read(m_timeStamp);
	inStream.Read(m_deltaTime);
	m_inputState.Read(inStream);
}
