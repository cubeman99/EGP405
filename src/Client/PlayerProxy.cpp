#include "PlayerProxy.h"
#include <GameLib/math/MathLib.h>

PlayerProxy::PlayerProxy(PlayerID playerId) :
	m_playerId(playerId),
	m_lastStateTimeStamp(0)
{
	m_interpolationDelay = 0.1f; // 100 ms.
}

void PlayerProxy::AddStateIfNew(float timeStamp, const Vector2f& position, const Vector2f& velocity)
{
	if (timeStamp > m_lastStateTimeStamp)
	{
		m_lastStateTimeStamp = timeStamp;
		m_stateQueue.push_back(PlayerState(timeStamp, position, velocity));
		m_interpolationState.SetTimeStamp(timeStamp);
	}
}

void PlayerProxy::ClearInterpolation()
{
	m_lastStateTimeStamp = 0;
	m_interpolationState.SetTimeStamp(0);
	m_stateQueue.clear();
}

void PlayerProxy::UpdateInterpolation(float timeDelta)
{
	float currentTimeStamp = m_interpolationState.GetTimeStamp();
	float delayedTimeStamp = currentTimeStamp - m_interpolationDelay;

	// Interpolate between states.
	for (unsigned int i = 1; i < m_stateQueue.size() + 1; i++)
	{
		if (i == m_stateQueue.size())
		{
			// Remove old and unnecessary snapshots.
			for (unsigned int j = 0; j < i - 1; j++)
				m_stateQueue.erase(m_stateQueue.begin());
		}
		else if (delayedTimeStamp <= m_stateQueue[i].GetTimeStamp())
		{
			// Interpolate position & velocity between these two states.
			float lerpFactor = (delayedTimeStamp - m_stateQueue[i - 1].GetTimeStamp()) /
				(m_stateQueue[i].GetTimeStamp() - m_stateQueue[i - 1].GetTimeStamp());
			
			m_interpolationState.SetPosition(Vector2f::Lerp(
				m_stateQueue[i - 1].GetPosition(),
				m_stateQueue[i].GetPosition(),
				lerpFactor));
			m_interpolationState.SetVelocity(Vector2f::Lerp(
				m_stateQueue[i - 1].GetVelocity(),
				m_stateQueue[i].GetVelocity(),
				lerpFactor));

			// Remove old and unnecessary snapshots.
			for (unsigned int j = 0; j < i - 1; j++)
				m_stateQueue.erase(m_stateQueue.begin());

			break;
		}
	}

	// Check if there is nothing to interpolate between.
	if (m_stateQueue.size() == 1)
	{
		m_interpolationState.SetPosition(m_stateQueue.front().GetPosition());
		m_interpolationState.SetVelocity(m_stateQueue.front().GetVelocity());
		m_interpolationState.SetTimeStamp(m_stateQueue.front().GetTimeStamp());
	}

	// Advance the current time-stamp, in between snapshots.
	if (m_stateQueue.size() > 1)
	{
		m_interpolationState.SetTimeStamp(currentTimeStamp + timeDelta);
	}
}

