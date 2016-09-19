#include "color.h"

// NOTE: this is code I wrote back in 2013.

// ========== CONSTANT COLOR FUNCTIONS ========== //

// Return whether the given color value is of light intensity.
bool isLight(int color)
{
	return color >= NUM_BASE_COLORS;
}

// Return whether the given color value is of dark intensity.
bool isDark(int color)
{
	return color < NUM_BASE_COLORS;
}

// Return the given color as light intensity.
int toLight(int color)
{
	if (color >= NUM_BASE_COLORS)
		return color;
	return color + NUM_BASE_COLORS;
}

// Return the given color as dark intensity.
int toDark(int color)
{
	if (color < NUM_BASE_COLORS)
		return color;
	return color - NUM_BASE_COLORS;
}

// Return the given color as the opposite intensity.
int inverseIntensity(int color)
{
	if (color < NUM_BASE_COLORS)
		return color + NUM_BASE_COLORS;
	return color - NUM_BASE_COLORS;
}

// Return the opposite of the given color.
int getInverse(int color)
{
	return (NUM_COLORS - 1 - color);
}



// ================== COLOR =================== //

Color::Color(int foreground, int background) :
	mValue(foreground + (background << 4))
{
}

Color::Color(const Color& copy) :
	mValue(copy.mValue)
{
}

// Return the background color value.
int Color::getBackground()
{
	return mValue >> 4;
}

// Return the foreground color value.
int Color::getForeground()
{
	return mValue - (getBackground() << 4);
}

// Return the combined color value.
int Color::getValue()
{
	return mValue;
}

// Set the background color value.
void Color::setBackground(int value)
{
	mValue = getForeground() + (value << 4);
}

// Set the foreground color value.
void Color::setForeground(int value)
{
	mValue = value + (getBackground() << 4);
}

// Invert the color.
void Color::invert()
{
	setForeground(getInverse(getForeground()));
	setBackground(getInverse(getBackground()));
}

// Set this color from another.
void Color::operator=(const Color& c)
{
	mValue = c.mValue;
}

// Test for equality.
bool Color::operator==(const Color& c)
{
	return mValue == c.mValue;
}

// Test for inequality.
bool Color::operator!=(const Color& c)
{
	return mValue != c.mValue;
}
