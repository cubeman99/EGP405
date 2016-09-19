#include "menu.h"
#include <sstream>
#include "menu_system.h"

using namespace std;


Menu::Menu() :
	m_title("")
{
}

void Menu::SetTitle(const std::string& title)
{
	m_title = title;
}

void Menu::AddOption(int number, const std::string& option)
{
	m_options[number] = option;
}

string Menu::GetMenuText()
{
	stringstream str;

	str << m_title << endl;

	for (OptionMap::iterator it = m_options.begin();
		it != m_options.end(); it++)
	{
		str << it->first << ") " << it->second << endl;
	}

	return str.str();
}
