#include "menu.h"
#include <sstream>
#include "menu_system.h"

using namespace std;


Menu::Menu() :
	m_title(""),
	m_description("")
{
}

std::string Menu::GetTitle() const
{
	return m_title;
}

std::string Menu::GetDescription() const
{
	return m_description;
}

std::string Menu::GetOption(int index) const
{
	return m_options[index];
}

int Menu::GetOptionCount() const
{
	return (int) m_options.size();
}

void Menu::SetTitle(const std::string& title)
{
	m_title = title;
}

void Menu::AddOption(const std::string& option)
{
	m_options.push_back(option);
}
