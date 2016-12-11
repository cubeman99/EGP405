#include "PlayerState.h"
#include <GameLib/math/MathLib.h>


void PlayerState::Serialize(RakNet::BitStream& bsOut, const GameConfig& gameConfig) const
{
	bool isOnGround = (Math::Abs(m_position.y - gameConfig.view.floorY) < 0.01f);

	// Time stamp.
	bsOut.Write(m_timeStamp);

	// X position.
	// (never compres).
	bsOut.Write(m_position.x);

	// Y position.
	// Compress if on the ground.
	if (!isOnGround)
	{
		bsOut.Write0();
		bsOut.Write(m_position.y);
	}
	else
	{
		bsOut.Write1();
	}

	// Velocity.
	// Component-wise compress if zero.
	for (int i = 0; i < 2; i++)
	{
		if (m_velocity[i] != 0.0f)
		{
			bsOut.Write0();
			bsOut.Write(m_velocity[i]);
		}
		else
		{
			bsOut.Write1();
		}
	}
}

void PlayerState::Deserialize(RakNet::BitStream& bsIn, const GameConfig& gameConfig)
{
	// Time stamp.
	bsIn.Read(m_timeStamp);

	// X position.
	// (never compressed).
	bsIn.Read(m_position.x);

	// Y position.
	// Compressed if on the ground.
	if (bsIn.ReadBit())
	{
		m_position.y = gameConfig.view.floorY;
	}
	else
	{
		bsIn.Read(m_position.y);
	}

	// Velocity.
	// Component-wise compressed if zero.
	for (int i = 0; i < 2; i++)
	{
		if (bsIn.ReadBit())
		{
			m_velocity[i] = 0.0f;
		}
		else
		{
			bsIn.Read(m_velocity[i]);
		}
	}
}
