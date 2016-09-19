#ifndef _COLOR_H_
#define _COLOR_H_

// NOTE: this is code I wrote back in 2013.

// All Console Colors:
const int BLACK = 0;
const int NAVY = 1;
const int GREEN = 2;
const int TEAL = 3;
const int MAROON = 4;
const int PURPLE = 5;
const int OLIVE = 6;
const int LIGHT_GRAY = 7;
const int GRAY = 8;
const int BLUE = 9;
const int LIME = 10;
const int CYAN = 11;
const int RED = 12;
const int MAGENTA = 13;
const int YELLOW = 14;
const int WHITE = 15;
const int NUM_BASE_COLORS = 8;
const int NUM_COLORS = 16;

// Color Functions:
bool isLight(int color);
bool isDark(int color);
int toLight(int color);
int toDark(int color);
int inverseIntensity(int color);
int getInverse(int color);


// A Color consists of a foreground and background color value.
class Color
{
public:
	Color(int foreground, int background);
	Color(const Color& copy);
	~Color() {};

	int getBackground();
	int getForeground();
	int getValue();

	void setBackground(int value);
	void setForeground(int value);
	void invert();

	void operator=(const Color& c);
	bool operator==(const Color& c);
	bool operator!=(const Color& c);

private:
	int mValue;
};

#endif _COLOR_H_