#ifndef _MENU_H_
#define _MENU_H_

#include <string>
#include <vector>

class MenuSystem;
class GameData;
struct PlayerInfo;


// Used to handle menu composition for the server.
class Menu
{
public:
	Menu();

	std::string GetTitle() const;
	std::string GetDescription() const;
	std::string GetOption(int index) const;
	int GetOptionCount() const;

	// Code to perform on the server when an option is chosen.
	virtual void OnEnter(GameData* game, PlayerInfo* player) = 0;
	virtual void OnChoose(int option, GameData* game, PlayerInfo* player) = 0;

private:
	std::vector<std::string> m_options;

protected:
	void SetTitle(const std::string& title);
	void AddOption(const std::string& option);

	std::string m_title;
	std::string m_description;
};


#endif // _MENU_H_