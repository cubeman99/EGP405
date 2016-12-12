#include "EntityStateHistory.h"


EntityStateHistory::EntityStateHistory(int entityId) :
	m_entityId(entityId),
	m_lastTimeStamp(0)
{
}

void EntityStateHistory::AddState(float interpolationDelay, float timeStamp, const Vector2f& position, const Vector2f& velocity)
{
	float delayedTimeStamp = m_state.GetTimeStamp() - interpolationDelay;

	// First, remove any old/unneeded state snapshots.
	for (int i = (int) m_buffer.size() - 1; i > 0; i--)
	{
		if (m_buffer[i].GetTimeStamp() <= delayedTimeStamp)
		{
			for (int j = i - 1; j >= 0; j--)
				m_buffer.erase(m_buffer.begin());
			break;
		}
	}

	// Add the new state snapshots to the history buffer.
	if (timeStamp > m_lastTimeStamp)
	{
		// Instant state up date of too far apart in space OR time.
		if (timeStamp - m_lastTimeStamp > 0.5f ||
			m_state.GetPosition().DistTo(position) > 50)
		{
			m_state.SetTimeStamp(timeStamp);
		}
		else if (m_buffer.size() > 0)
		{
			m_state.SetTimeStamp(m_buffer.back().GetTimeStamp());
		}

		m_lastTimeStamp = timeStamp;
		m_buffer.push_back(EntityState(timeStamp, m_entityId, position, velocity));
	}
}

void EntityStateHistory::ClearBuffer()
{
	m_lastTimeStamp = 0;
	m_state.SetTimeStamp(0);
	m_buffer.clear();
}

void EntityStateHistory::Update(float timeDelta, float interpolationDelay)
{
	float currentTimeStamp = m_state.GetTimeStamp();
	float delayedTimeStamp = currentTimeStamp - interpolationDelay;

	if (m_buffer.size() > 1 && m_buffer.back().GetTimeStamp() <= currentTimeStamp)
	{
		m_state = m_buffer.back();
	}
	else
	{
		// Interpolate between states.
		for (int i = (int) m_buffer.size() - 2; i >= 0; i--)
		{
			if (m_buffer[i].GetTimeStamp() <= currentTimeStamp)
			{
				// Interpolate position & velocity between these two states.
				float lerpFactor = (currentTimeStamp - m_buffer[i].GetTimeStamp()) /
					(m_buffer[i + 1].GetTimeStamp() - m_buffer[i].GetTimeStamp());
				m_state.SetPosition(Vector2f::Lerp(m_buffer[i].GetPosition(),
					m_buffer[i + 1].GetPosition(), lerpFactor));
				m_state.SetVelocity(Vector2f::Lerp(m_buffer[i].GetVelocity(),
					m_buffer[i + 1].GetVelocity(), lerpFactor));
				break;
			}
		}
	}

	// Check if there is nothing to interpolate between.
	if (m_buffer.size() == 1)
	{
		m_state = m_buffer.front();
	}

	// Advance the current time-stamp, in between snapshots.
	if (m_buffer.size() > 1)
	{
		m_state.SetTimeStamp(currentTimeStamp + timeDelta);
	}
}

