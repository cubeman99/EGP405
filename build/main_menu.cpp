#include "main_menu.h"
#include "menu_system.h"
#include "game.h"

using namespace std;

MainMenu::MainMenu()
{
	m_title = "Main Menu";

	AddOption(0, "Quit");
	AddOption(1, "Player Info");
	AddOption(2, "Gamble (50 gold)");
	AddOption(3, "Server Bank...");
}

void MainMenu::OnChoose(int option, GameData* game, PlayerInfo* player)
{
	switch (option)
	{
	case 0: // Quit
	{
		player->m_menuSystem.ExitMenu(); // This will tell the client to quit the game.
		break;
	}
	case 1:
	{
		player->m_outputStream << "Player Info:" << endl;
		player->m_outputStream << " * Health: " << player->m_gold << endl;
		player->m_outputStream << " * Gold: " << player->m_gold << endl;
		break;
	}
	case 2: // Gamble
	{
		if (player->m_gold < 50)
		{
			player->m_outputStream <<"You don't have enough gold to gamble!" << endl;
		}
		else
		{
			bool win = (rand() % 10 < 3); // 30% chance to win (>RIGGED)

			if (win)
			{
				player->m_gold += 50;
				player->m_outputStream << "Congratulations! You won 50 gold!" << endl;
				player->m_outputStream << "You now have " << player->m_gold << " gold!" << endl;
			}
			else
			{
				player->m_gold -= 50;
				player->m_outputStream << "Doh! You lost 50 gold!" << endl;
				player->m_outputStream << "You now have " << player->m_gold << " gold." << endl;
			}
		}
		break;
	}
	case 3: // Server Bank
	{
		player->m_outputStream << "Welcome to the bank!" << endl << endl;

		player->m_outputStream << "There is " << game->m_bankGold << " gold in the vault." << endl;
		player->m_outputStream << "You have " << player->m_gold << " gold." << endl;

		player->m_menuSystem.EnterMenu(&m_bankMenu);
		break;
	}
	default:
		player->m_outputStream << "Invalid input" << endl;
	}
}