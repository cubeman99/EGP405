#include "EntityInterpolator.h"


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