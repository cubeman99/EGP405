#ifndef _MENU_SYSTEM_H_
#define _MENU_SYSTEM_H_

#include <stack>

class Menu;

// MenuSystem - a simple stack of menu objects.
class MenuSystem
{
public:
	MenuSystem();

	Menu* GetCurrentMenu();

	void Init(Menu* menu);
	void EnterMenu(Menu* menu);
	void ExitMenu();
	
private:
	std::stack<Menu*> m_menuStack;
};


#endif // _MENU_SYSTEM_H_