#include "EntityState.h"


EntityState::EntityState()
{
}

EntityState::EntityState(float timeStamp, int entityId, const Vector2f& position, const Vector2f& velocity) :
	m_timeStamp(timeStamp),
	m_entityId(entityId),
	m_position(position),
	m_velocity(velocity)
{
}

EntityState::EntityState(int entityId, const Vector2f& position, const Vector2f& velocity) :
	m_entityId(entityId),
	m_position(position),
	m_velocity(velocity)
{
}

void EntityState::Serialize(RakNet::BitStream& outStream) const
{
	outStream.Write(m_entityId);
	outStream.Write(m_timeStamp);
	outStream.Write(m_position);
	outStream.Write(m_velocity);
}

void EntityState::Deserialize(RakNet::BitStream& inStream)
{
	inStream.Read(m_entityId);
	inStream.Read(m_timeStamp);
	inStream.Read(m_position);
	inStream.Read(m_velocity);
}
