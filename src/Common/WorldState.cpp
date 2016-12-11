#include "WorldState.h"
#include <GameLib/math/MathLib.h>


void EntityState::Serialize(RakNet::BitStream& outStream) const
{
	outStream.Write(m_entityId);
	outStream.Write(m_position);
	outStream.Write(m_velocity);
}

void EntityState::Deserialize(RakNet::BitStream& inStream)
{
	inStream.Read(m_entityId);
	inStream.Read(m_position);
	inStream.Read(m_velocity);
}



WorldState::WorldState()
{
}

WorldState::WorldState(float timeStamp) :
	m_timeStamp(timeStamp)
{
}

EntityState* WorldState::GetEntityState(int entityId)
{
	auto it = m_entityStates.find(entityId);
	if (it != m_entityStates.end())
		return &it->second;
	return nullptr;
}

void WorldState::SetEntityState(const EntityState& entityState)
{
	m_entityStates[entityState.GetEntityId()] = entityState;
}

void WorldState::Clear()
{
	m_entityStates.clear();
}

void WorldState::Interpolate(WorldState* a, WorldState* b, float t)
{
	int maxEntityStates = Math::Max(a->m_entityStates.size(), b->m_entityStates.size());
	//m_entityStates.resize(maxEntityStates);
	m_entityStates.clear();

	// Add all states from a.
	for (auto it = a->m_entityStates.begin(); 
		it != a->m_entityStates.end(); it++)
	{
		m_entityStates[it->first] = it->second;
	}

	// Add all states from b.
	for (auto it = b->m_entityStates.begin();
		it != b->m_entityStates.end(); it++)
	{
		EntityState* astate = GetEntityState(it->first);

		if (astate != nullptr)
		{
			// Interpolate between a and b.
			EntityState* bstate = &it->second;
			astate->SetPosition(Vector2f::Lerp(astate->GetPosition(), bstate->GetPosition(), t));
			astate->SetVelocity(Vector2f::Lerp(astate->GetVelocity(), bstate->GetVelocity(), t));
				
		}
		else
		{
			m_entityStates[it->first] = it->second;
		}
	}
}

void WorldState::Set(WorldState* other)
{
	m_timeStamp = other->m_timeStamp;
	m_entityStates.clear();

	for (auto it = other->m_entityStates.begin();
		it != other->m_entityStates.end(); it++)
	{
		m_entityStates[it->first] = it->second;
	}
}

void WorldState::Serialize(RakNet::BitStream& outStream) const
{
	outStream.Write((int) m_entityStates.size());
}

void WorldState::Deserialize(RakNet::BitStream& inStream)
{
	int numEntityStates;
	inStream.Read(numEntityStates);

	EntityState state;
	for (int i = 0; i < numEntityStates; i++)
	{
		state.Deserialize(inStream);
		m_entityStates[state.GetEntityId()] = state;
	}
}