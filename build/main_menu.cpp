#include "main_menu.h"
#include "menu_system.h"
#include "game.h"

using namespace std;

enum
{
	OPT_PLAYER_INFO,
	OPT_GAMBLE,
	OPT_BANK_MENU,
	OPT_QUIT,
};

MainMenu::MainMenu()
{
	m_title = "Main Menu";

	AddOption("Player Info");
	AddOption("Gamble (50 gold)");
	AddOption("Server Bank...");
	AddOption("Quit");
}

void MainMenu::OnEnter(GameData* game, PlayerInfo* player)
{
	ostringstream str;

	str << "Welcome to the server!" << endl;

	m_description = str.str();
}


void MainMenu::OnChoose(int option, GameData* game, PlayerInfo* player)
{
	ostringstream str;

	switch (option)
	{
	case OPT_PLAYER_INFO:
	{
		str << "Player Info:" << endl;
		str << " * Health: " << player->m_gold << endl;
		str << " * Gold: " << player->m_gold << endl;
		break;
	}
	case OPT_GAMBLE: // Gamble
	{
		if (player->m_gold < 50)
		{
			str << "You don't have enough gold to gamble!" << endl;
		}
		else
		{
			bool win = (rand() % 10 < 3); // 30% chance to win (>RIGGED)

			if (win)
			{
				player->m_gold += 50;
				str << "Congratulations! You won 50 gold!" << endl;
				str << "You now have " << player->m_gold << " gold!" << endl;
			}
			else
			{
				player->m_gold -= 50;
				str << "Doh! You lost 50 gold!" << endl;
				str << "You now have " << player->m_gold << " gold." << endl;
			}
		}
		break;
	}
	case OPT_BANK_MENU: // Server Bank
	{
		player->m_menuSystem.EnterMenu(&m_bankMenu, game, player);
		break;
	}
	case OPT_QUIT: // Quit
	{
		// This will tell the client to quit the game.
		player->m_menuSystem.ExitMenu(game, player);
		break;
	}
	default:
		str << "Invalid input" << endl;
	}

	m_description = str.str();
}