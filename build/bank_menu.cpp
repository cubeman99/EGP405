#include "bank_menu.h"
#include "menu_system.h"
#include "game.h"

using namespace std;

enum
{
	OPT_DEPOSIT_10G,
	OPT_DEPOSIT_ALL,
	OPT_WITHDRAW_10G,
	OPT_WITHDRAW_ALL,
	OPT_BACK,
};

BankMenu::BankMenu()
{
	m_title = "Server Bank Menu";

	AddOption("Deposit 10 gold");
	AddOption("Deposit all");
	AddOption("Withdraw 10 gold");
	AddOption("Withdraw all");
	AddOption("Back");
}

void BankMenu::OnEnter(GameData* game, PlayerInfo* player)
{
	ostringstream str;

	str << "Welcome to the bank!" << endl << endl
	    << "There is " << game->m_bankGold << " gold in the vault." << endl
	    << "You have " << player->m_gold << " gold." << endl;

	m_description = str.str();
}

void BankMenu::OnChoose(int option, GameData* game, PlayerInfo* player)
{
	ostringstream str;

	switch (option)
	{
	case OPT_DEPOSIT_10G:
	{
		if (player->m_gold >= 10)
		{
			str << "Deposited 10 gold." << endl << endl;
			player->m_gold -= 10;
			game->m_bankGold += 10;
		}
		else
		{
			str << "You don't have any gold to deposit!" << endl << endl;
		}
		break;
	}
	case OPT_DEPOSIT_ALL:
	{
		if (player->m_gold > 0)
		{
			str << "Deposited " << player->m_gold << " gold." << endl << endl;
			game->m_bankGold += player->m_gold;
			player->m_gold = 0;
		}
		else
		{
			str << "You don't have any gold to deposit!" << endl << endl;
		}
		break;
	}
	case OPT_WITHDRAW_10G:
	{
		if (game->m_bankGold >= 10)
		{
			str << "Withdrew 10 gold." << endl << endl;
			player->m_gold += 10;
			game->m_bankGold -= 10;
		}
		else
		{
			str << "There is no gold in the vault!" << endl << endl;
		}
		break;
	}
	case OPT_WITHDRAW_ALL:
	{
		if (game->m_bankGold > 0)
		{
			str << "Withdrew " << game->m_bankGold << " gold." << endl << endl;
			player->m_gold += game->m_bankGold;
			game->m_bankGold = 0;
		}
		else
		{
			str << "There is no gold in the vault!" << endl << endl;
		}
		break;
	}
	case OPT_BACK:
	{
		player->m_menuSystem.ExitMenu(game, player);
		break;
	}
	default:
		str << "Invalid input" << endl;
	}

	str << "There is " << game->m_bankGold << " gold in the vault." << endl;
	str << "You have " << player->m_gold << " gold." << endl;

	m_description = str.str();
}