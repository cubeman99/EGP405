#include "menu_display.h"
#include "color.h"
#include <iostream>


MenuDisplay::MenuDisplay()
{
	m_cursorIndex = 0;
	m_title = "Menu Title";
	m_description = "Descrfiption goes here tyayaysd asas das asd!";
	m_options.push_back("Option number 1");
	m_options.push_back("Option number 2");
	m_options.push_back("Option number 3");
	m_options.push_back("Quit");

	m_newMenu = true;
}

int MenuDisplay::GetCursorIndex() const
{
	return m_cursorIndex;
}

void MenuDisplay::SetTitle(const std::string& title)
{
	m_title = title;
}

void MenuDisplay::SetDescription(const std::string& description)
{
	m_description = description;
	m_newMenu = true;
}

void MenuDisplay::NewMenu(
	const std::string& title,
	const std::string& description,
	const std::vector<std::string>& options)
{
	m_title = title;
	m_description = description;
	m_options = options;
	m_cursorIndex = 0;
	m_newMenu = true;
}

void MenuDisplay::MoveCursorNext()
{
	if (m_options.size() > 0)
		m_cursorIndex = (m_cursorIndex + 1) % m_options.size();
}

void MenuDisplay::MoveCursorBack()
{
	if (m_options.size() > 0)
		m_cursorIndex = (m_cursorIndex + (int)m_options.size() - 1) % (int)m_options.size();
}

void MenuDisplay::Display(HANDLE handle)
{
	Color colorDefault(WHITE, BLACK);
	Color colorTitle(YELLOW, BLACK);
	Color colorDescription(WHITE, BLACK);
	Color colorOption(WHITE, BLACK);
	Color colorOptionHighlighted(BLACK, WHITE);

	// Clear the whole screen if the entire menu changed.
	if (m_newMenu)
	{
		SetConsoleTextAttribute(handle, colorDefault.getValue());
		system("cls");
		m_newMenu = false;
	}

	// Print the title.
	SetConsoleTextAttribute(handle, colorTitle.getValue());
	SetConsoleCursorPosition(handle, { 0, 2 });
	std::cout << m_title;

	// Print the description.
	SetConsoleTextAttribute(handle, colorDescription.getValue());
	SetConsoleCursorPosition(handle, { 0, 5 });
	std::cout << m_description << std::endl << std::endl;

	// Print the menu options.
	for (int i = 0; i < (int) m_options.size(); i++)
	{
		SetConsoleTextAttribute(handle, colorOption.getValue());
		std::cout << " ";

		// Check if this option is highlighted.
		if (m_cursorIndex == i)
			SetConsoleTextAttribute(handle, colorOptionHighlighted.getValue());

		//SetConsoleCursorPosition(handle, { 3, 10 + ((short) i * 2) });
		std::cout << "[ " << m_options[i] << " ]" << std::endl << std::endl;

		SetConsoleTextAttribute(handle, colorOption.getValue());
	}
}
