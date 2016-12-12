#ifndef _COLOR_SCHEME_H_
#define _COLOR_SCHEME_H_

#include <graphics/Color.h>
#include <math/Vector2f.h>
#include <vector>


struct ColorScheme
{
	ColorScheme() {}

	Color skyColor;
	Color groundColor;
	Color netColor;
	Color ballColor;

	struct
	{
		Color eyeColor;
		Color pupilColor;
		std::vector<Color> bodyColors;

	} slime;

	struct
	{
		Color scoreTextColor;
		Color gameInfoTextColor;
		Color menuPromptTextColor;

		struct 
		{
			Color textColor;
			Color backgroundColor;
		} joinTeamButton;

		struct
		{
			Color outlineColor;
		} chooseColorButton;

	} ui;
};


#endif // _COLOR_SCHEME_H_