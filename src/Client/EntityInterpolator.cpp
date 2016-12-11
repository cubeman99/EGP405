#include "EntityInterpolator.h"


EntityInterpolator::EntityInterpolator()
{
	m_interpolationDelay = 0.1f;
}

EntityInterpolator::~EntityInterpolator()
{
	for (auto it = m_snapshots.begin(); it != m_snapshots.end(); it++)
		delete *it;
	m_snapshots.clear();
}

WorldState* EntityInterpolator::CreateSnapshotIfNew(float timeStamp)
{
	float currentTimeStamp = m_interpolatedState.GetTimeStamp();
	float delayedTimeStamp = currentTimeStamp - m_interpolationDelay;

	WorldState* recycledWorldState = nullptr;

	// First, remove any old/unneeded snapshots.
	for (unsigned int i = m_snapshots.size() - 1; i > 0; i--)
	{
		if (m_snapshots[i]->GetTimeStamp() <= delayedTimeStamp)
		{
			for (unsigned int j = i - 1; j >= 0; j--)
			{
				if (recycledWorldState == nullptr)
					recycledWorldState = m_snapshots.front();
				else
					delete m_snapshots.front();
				m_snapshots.erase(m_snapshots.begin());
			}
			break;
		}
	}

	// Add the new world snapshot.
	if (timeStamp > m_lastSnapshotTimeStamp)
	{
		m_lastSnapshotTimeStamp = timeStamp;
		if (recycledWorldState == nullptr)
			recycledWorldState = new WorldState();
		recycledWorldState->SetTimeStamp(timeStamp);
		m_snapshots.push_back(recycledWorldState);
	}

	return recycledWorldState;
}

void EntityInterpolator::ClearSnapshots()
{
	m_lastSnapshotTimeStamp = 0;
	m_interpolatedState.SetTimeStamp(0);
	m_snapshots.clear();
}

void EntityInterpolator::Update(float timeDelta, GameWorld* gameWorld)
{
	float currentTimeStamp = m_interpolatedState.GetTimeStamp();
	float delayedTimeStamp = currentTimeStamp - m_interpolationDelay;

	// Interpolate between states.
	for (unsigned int i = m_snapshots.size() - 2; i >= 0; i--)
	{
		if (m_snapshots[i]->GetTimeStamp() <= delayedTimeStamp)
		{
			// Interpolate position & velocity between these two states.
			float lerpFactor = (delayedTimeStamp - m_snapshots[i]->GetTimeStamp()) /
				(m_snapshots[i + 1]->GetTimeStamp() - m_snapshots[i]->GetTimeStamp());
			m_interpolatedState.Interpolate(
				m_snapshots[i], m_snapshots[i + 1], lerpFactor);
			break;
		}
	}

	// Check if there is nothing to interpolate between.
	if (m_snapshots.size() == 1)
	{
		m_interpolatedState.Set(m_snapshots.front());
	}

	// Advance the current time-stamp, in between snapshots.
	if (m_snapshots.size() > 1)
	{
		m_interpolatedState.SetTimeStamp(currentTimeStamp + timeDelta);
	}
}
