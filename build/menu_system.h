#ifndef _MENU_SYSTEM_H_
#define _MENU_SYSTEM_H_

#include <stack>

class Menu;
class GameData;
struct PlayerInfo;

// MenuSystem - a simple stack of menu objects (used for the server).
class MenuSystem
{
public:
	MenuSystem();

	Menu* GetCurrentMenu();

	void Init(Menu* menu, GameData* game, PlayerInfo* player);
	void EnterMenu(Menu* menu, GameData* game, PlayerInfo* player);
	void ExitMenu(GameData* game, PlayerInfo* player);
	
private:
	std::stack<Menu*> m_menuStack;
};


#endif // _MENU_SYSTEM_H_