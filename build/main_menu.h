#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "menu.h"
#include "bank_menu.h"

class MainMenu: public Menu
{
public:
	MainMenu();

	void OnChoose(int option, GameData* game, PlayerInfo* player) override;

private:
	BankMenu m_bankMenu;
};


#endif // _MENU_H_