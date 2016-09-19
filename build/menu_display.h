#ifndef _MENU_DISPLAY_H_
#define _MENU_DISPLAY_H_

#include <Windows.h>
#include <vector>
#include <string>


// Used to display and navigate a menu for the client.
class MenuDisplay
{
public:
	MenuDisplay();

	int GetCursorIndex() const;

	void SetTitle(const std::string& title);
	void SetDescription(const std::string& description);
	void NewMenu(
		const std::string& title,
		const std::string& description,
		const std::vector<std::string>& options);

	void MoveCursorNext();
	void MoveCursorBack();

	void Display(HANDLE handle);

private:
	int m_cursorIndex;
	std::vector<std::string> m_options;
	std::string m_title;
	std::string m_description;

	bool m_newMenu;
};

#endif // _MENU_DISPLAY_H_
