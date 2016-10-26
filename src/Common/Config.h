#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <graphics/Color.h>
#include <math/Vector2f.h>
#include <vector>


struct GameConfig
{
	GameConfig();

	struct
	{
		float radius;
		float gravity;
		float maxSpeed;
		float serveHeight;
	} ball;

	struct
	{
		float radius;
		float gravity;
		float jumpSpeed;
		float moveSpeed;
	} slime;

	struct
	{
		float width;
		float height;
		float depthBelowGround;
	} net;

	struct
	{
		float width;
		float height;
		float floorY;
	} view;

	float scorePauseSeconds;
	int maxPlayers;
};


struct ColorScheme
{
	ColorScheme();

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


#endif // _CONFIG_H_