#include "keyboard.h"

// NOTE: this is code I wrote back in 2013.

// ================== KEYBOARD ================== //

std::vector<Key*> Keyboard::mpKeys;
Key* Keyboard::KEY_ENTER = new Key(VK_RETURN);
Key* Keyboard::KEY_SPACE = new Key(VK_SPACE);
Key* Keyboard::KEY_SHIFT = new Key(VK_SHIFT);
Key* Keyboard::KEY_CONTROL = new Key(VK_CONTROL);
Key* Keyboard::KEY_RIGHT = new Key(VK_RIGHT);
Key* Keyboard::KEY_LEFT = new Key(VK_LEFT);
Key* Keyboard::KEY_UP = new Key(VK_UP);
Key* Keyboard::KEY_DOWN = new Key(VK_DOWN);
Key* Keyboard::KEY_ESCAPE = new Key(VK_ESCAPE);
Key* Keyboard::KEY_BACKSPACE = new Key(VK_BACK);
Key* Keyboard::KEY_DELETE = new Key(VK_DELETE);
Key* Keyboard::KEY_PAGE_UP = new Key(VK_PRIOR);
Key* Keyboard::KEY_PAGE_DOWN = new Key(VK_NEXT);
Key* Keyboard::KEY_F5 = new Key(VK_F5);


// Add all the keys to a list.
void Keyboard::initialize()
{
	mpKeys.push_back(KEY_ENTER);
	mpKeys.push_back(KEY_SPACE);
	mpKeys.push_back(KEY_SHIFT);
	mpKeys.push_back(KEY_CONTROL);
	mpKeys.push_back(KEY_RIGHT);
	mpKeys.push_back(KEY_LEFT);
	mpKeys.push_back(KEY_UP);
	mpKeys.push_back(KEY_DOWN);
	mpKeys.push_back(KEY_ESCAPE);
	mpKeys.push_back(KEY_BACKSPACE);
	mpKeys.push_back(KEY_DELETE);
	mpKeys.push_back(KEY_F5);
	mpKeys.push_back(KEY_PAGE_UP);
	mpKeys.push_back(KEY_PAGE_DOWN);
}

// Update all key states.
void Keyboard::update()
{
	for (int i = 0; i < (int)mpKeys.size(); i++)
		mpKeys[i]->update();
}

// Delete all keys.
void Keyboard::terminate()
{
	for (int i = 0; i < (int)mpKeys.size(); i++)
		delete mpKeys[i];
}



// ===================== KEY ==================== //

// Create a key from a virtual key code
Key::Key(int keyCode) :
	mKeyCode(keyCode),
	mTypeTimer(0),
	mState(false),
	mStatePrevious(false)
{
}

// Create a key from a character
Key::Key(char c) :
	mKeyCode((int)c),
	mTypeTimer(0),
	mState(false),
	mStatePrevious(false)
{
}

// Whether the key is down.
bool Key::down()
{
	return mState;
}

// Whether the key is pressed.
bool Key::pressed()
{
	return (mState && !mStatePrevious);
}

// Whether the key is released.
bool Key::released()
{
	return (!mState && mStatePrevious);
}

// Whether the key is typed.
bool Key::typed()
{
	return (pressed() || (down() && mTypeTimer > KEY_TYPE_DELAY));
}

// Update the key's state.
void Key::update()
{
	mStatePrevious = mState;
	mState = (GetAsyncKeyState(mKeyCode) != 0);
	if (mState)
		mTypeTimer++;
	else
		mTypeTimer = 0;
}