#include "menu_system.h"
#include "menu.h"

MenuSystem::MenuSystem()
{

}

Menu* MenuSystem::GetCurrentMenu()
{
	if (m_menuStack.size() == 0)
		return NULL;
	return m_menuStack.top();
}

void MenuSystem::Init(Menu* menu)
{
	// Clear the menu stack first.
	while (!m_menuStack.empty())
	{
		m_menuStack.pop();
	}

	m_menuStack.push(menu);
}

void MenuSystem::EnterMenu(Menu* menu)
{
	m_menuStack.push(menu);
}

void MenuSystem::ExitMenu()
{
	m_menuStack.pop();
}
