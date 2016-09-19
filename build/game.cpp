#include "game.h"

using namespace RakNet;


PlayerInfo::PlayerInfo(const RakNetGUID& guid) :
	m_guid(guid),
	m_health(100),
	m_gold(100)
{
}

PlayerInfo* GameData::ConnectPlayer(const RakNetGUID& guid)
{
	if (m_players.find(guid) == m_players.end())
	{
		// A new player has connected!

		PlayerInfo* player = new PlayerInfo(guid);

		player->m_menuSystem.Init(&m_mainMenu, this, player);
		player->m_health = 0;

		m_players[guid] = player;

		return player;
	}
	else
	{
		// This player has already connected before.

		return m_players[guid];
	}
}

PlayerInfo* GameData::GetPlayerInfo(const RakNetGUID& guid)
{
	return m_players[guid];
}


GameData::GameData() :
	m_bankGold(80)
{
}

GameData::~GameData()
{
	// Delete player info entries.
	for (PlayerInfoMap::iterator it = m_players.begin();
		it != m_players.end(); it++)
	{
		delete it->second;
	}
}

