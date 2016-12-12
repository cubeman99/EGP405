#include "EntityInterpolator.h"



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
		if (timeStamp - m_lastTimeStamp > 0.5f)
			m_state.SetTimeStamp(timeStamp);
		else if (m_buffer.size() > 0)
			m_state.SetTimeStamp(m_buffer.back().GetTimeStamp());
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



EntityInterpolator::EntityInterpolator()
{
	m_interpolationDelay = 0.1f;
}

EntityInterpolator::~EntityInterpolator()
{
	for (auto it = m_entityStateHistoryMap.begin();
		it != m_entityStateHistoryMap.end(); it++)
	{
		delete it->second;
	}
	m_entityStateHistoryMap.clear();

	for (auto it = m_snapshots.begin(); it != m_snapshots.end(); it++)
		delete *it;
	m_snapshots.clear();
}

EntityStateHistory* EntityInterpolator::GetEntityStateHistory(int entityId)
{
	auto it = m_entityStateHistoryMap.find(entityId);

	if (it != m_entityStateHistoryMap.end())
	{
		return it->second;
	}
	else
	{
		EntityStateHistory* esh = new EntityStateHistory(entityId);
		m_entityStateHistoryMap[entityId] = esh;
		return esh;
	}
}

void EntityInterpolator::AddStateSnapshot(int playerId, const PlayerState& playerState)
{
	EntityStateHistory* esh = GetEntityStateHistory(playerId);
	esh->AddState(m_interpolationDelay, playerState.GetTimeStamp(),
		playerState.GetPosition(), playerState.GetVelocity());
}

void EntityInterpolator::AddStateSnapshot(int entityId, float timeStamp, const Vector2f& position, const Vector2f& velocity)
{
	EntityStateHistory* esh = GetEntityStateHistory(entityId);
	esh->AddState(m_interpolationDelay,
		timeStamp, position, velocity);
}

void EntityInterpolator::AddStateSnapshots(WorldState& worldState)
{
	for (auto it = worldState.entity_states_begin();
		it != worldState.entity_states_end(); it++)
	{
		AddStateSnapshot(it->first,
			it->second.GetTimeStamp(),
			it->second.GetPosition(),
			it->second.GetVelocity());
	}
}

void EntityInterpolator::ClearSnapshots()
{
	for (auto it = m_entityStateHistoryMap.begin();
		it != m_entityStateHistoryMap.end(); it++)
	{
		it->second->ClearBuffer();
	}
}

void EntityInterpolator::Update(float timeDelta, GameWorld* gameWorld)
{
	// Update state interpolation per entity.
	for (auto it = m_entityStateHistoryMap.begin();
		it != m_entityStateHistoryMap.end(); it++)
	{
		it->second->Update(timeDelta, m_interpolationDelay);

		Slime* entity = gameWorld->GetPlayer(it->first);
		if (entity != nullptr)
		{
			entity->SetPosition(it->second->GetState().GetPosition());
			entity->SetVelocity(it->second->GetState().GetVelocity());
		}
	}
}