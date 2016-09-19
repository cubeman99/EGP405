#ifndef _BANK_MENU_H_
#define _BANK_MENU_H_

#include "menu.h"

class BankMenu : public Menu
{
public:
	BankMenu();

	void OnChoose(int option, GameData* game, PlayerInfo* player) override;

private:
};


#endif // _MENU_H_