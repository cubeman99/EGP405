#ifndef _GAME_H_
#define _GAME_H_

#include <map>
#include <RakNet/RakNetTypes.h>
#include <sstream>
#include "menu_system.h"
#include "main_menu.h"

//-----------------------------------------------------------------------------
// PlayerInfo - data for a single player/client
//-----------------------------------------------------------------------------
struct PlayerInfo
{
public:
	PlayerInfo(const RakNet::RakNetGUID& guid);

	RakNet::RakNetGUID m_guid;
	int m_health;
	int m_gold;
	MenuSystem m_menuSystem;
	std::ostringstream m_outputStream; // Used to build text to send back to the client.
};

//-----------------------------------------------------------------------------
// GameData - server-wide data for the game environment
//-----------------------------------------------------------------------------
class GameData
{
public:
	GameData();
	~GameData();

	PlayerInfo* ConnectPlayer(const RakNet::RakNetGUID& guid);
	PlayerInfo* GetPlayerInfo(const RakNet::RakNetGUID& guid);

private:
	typedef std::map<RakNet::RakNetGUID, PlayerInfo*> PlayerInfoMap;

	PlayerInfoMap m_players;
	MainMenu m_mainMenu;

public:
	int m_bankGold; // server-wide bank.
};


#endif // _GAME_H_