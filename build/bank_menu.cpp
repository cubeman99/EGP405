#include "bank_menu.h"
#include "menu_system.h"
#include "game.h"

using namespace std;

BankMenu::BankMenu()
{
	m_title = "Server Bank Menu";

	AddOption(0, "Back");
	AddOption(1, "Deposit 10 gold");
	AddOption(2, "Withdraw 10 gold");
}

void BankMenu::OnChoose(int option, GameData* game, PlayerInfo* player)
{
	switch (option)
	{
	case 0: // Back
	{
		player->m_menuSystem.ExitMenu();
		break;
	}
	case 1: // Deposit
	{
		if (player->m_gold >= 10)
		{
			player->m_outputStream << "Deposited 10 gold." << endl << endl;
			player->m_gold -= 10;
			game->m_bankGold += 10;
		}
		else
		{
			player->m_outputStream << "You don't have any gold to deposit!" << endl << endl;
		}
		break;
	}
	case 2: // Withdraw
	{
		if (game->m_bankGold >= 10)
		{
			player->m_outputStream << "Withdrew 10 gold." << endl << endl;
			player->m_gold += 10;
			game->m_bankGold -= 10;
		}
		else
		{
			player->m_outputStream << "There is no gold in the vault!" << endl << endl;
		}
		break;
	}
	default:
		player->m_outputStream << "Invalid input" << endl;
	}

	player->m_outputStream << "There is " << game->m_bankGold << " gold in the vault." << endl;
	player->m_outputStream << "You have " << player->m_gold << " gold." << endl;
}