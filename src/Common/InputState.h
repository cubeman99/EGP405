#ifndef _INPUT_STATE_H_
#define _INPUT_STATE_H_

#include <math/Vector2f.h>
#include <RakNet\BitStream.h>


class InputState
{
public:
	InputState();

	inline float GetDesiredHorizontalDelta() const { return m_desiredRightAmount - m_desiredLeftAmount; }
	inline bool IsJumping() const { return m_isJumping; }

	void Write(RakNet::BitStream& outStream) const;
	void Read(RakNet::BitStream& inStream);
	
private:
	friend class InputManager;

	float m_desiredRightAmount;
	float m_desiredLeftAmount;
	bool m_isJumping;
};


#endif // _INPUT_STATE_H_