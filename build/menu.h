#ifndef _MENU_H_
#define _MENU_H_

#include <map>
#include <string>

class MenuSystem;
class GameData;
struct PlayerInfo;


class Menu
{
public:
	Menu();

	// Code to perform on the server when an option is chosen.
	virtual void OnChoose(int option, GameData* game, PlayerInfo* player) = 0;

	// Gets the menu text string to display to the client.
	std::string GetMenuText();

private:
	typedef std::map<int, std::string> OptionMap;
	
	OptionMap m_options;

protected:
	void SetTitle(const std::string& title);
	void AddOption(int number, const std::string& option);

	std::string m_title;
};


#endif // _MENU_H_