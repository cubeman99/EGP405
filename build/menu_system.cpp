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

void MenuSystem::Init(Menu* menu, GameData* game, PlayerInfo* player)
{
	// Clear the menu stack first.
	while (!m_menuStack.empty())
	{
		m_menuStack.pop();
	}

	m_menuStack.push(menu);
	menu->OnEnter(game, player);
}

void MenuSystem::EnterMenu(Menu* menu, GameData* game, PlayerInfo* player)
{
	m_menuStack.push(menu);
	menu->OnEnter(game, player);
}

void MenuSystem::ExitMenu(GameData* game, PlayerInfo* player)
{
	m_menuStack.pop();

	if (m_menuStack.size() > 0)
		m_menuStack.top()->OnEnter(game, player);
}
