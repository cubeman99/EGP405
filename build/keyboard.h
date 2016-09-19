#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "Windows.h"
#include  <vector>

// NOTE: this is code I wrote back in 2013.

// The delay when a key's pressed before it begins typing continuously.
const int KEY_TYPE_DELAY = 15;


// Class to keep track of a single key.
class Key
{
public:
	Key(int keyCode);
	Key(char c);
	~Key() {};

	bool down();
	bool pressed();
	bool released();
	bool typed();

	void update();

private:
	bool mState;
	bool mStatePrevious;
	int mKeyCode;
	int mTypeTimer;
};


// Static class to update and hold keyboard keys.
class Keyboard
{
public:
	static Key* KEY_ENTER;
	static Key* KEY_SPACE;
	static Key* KEY_SHIFT;
	static Key* KEY_CONTROL;
	static Key* KEY_RIGHT;
	static Key* KEY_LEFT;
	static Key* KEY_UP;
	static Key* KEY_DOWN;
	static Key* KEY_BACKSPACE;
	static Key* KEY_ESCAPE;
	static Key* KEY_DELETE;
	static Key* KEY_F5;
	static Key* KEY_PAGE_UP;
	static Key* KEY_PAGE_DOWN;

	static void initialize();
	static void terminate();
	static void update();

private:
	static std::vector<Key*> mpKeys;
};


#endif // _KEYBOARD_H_