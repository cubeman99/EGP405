#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <graphics/Color.h>
#include <math/Vector2f.h>


struct Config
{
	Config();


	Color SKY_COLOR;
	Color GROUND_COLOR;
	Color NET_COLOR;

	// BALL:
	float BALL_RADIUS;
	float BALL_GRAVITY;
	float BALL_MAX_SPEED;
	float BALL_SERVE_HEIGHT;
	Color BALL_COLOR;

	// SLIME:
	float SLIME_RADIUS;
	float SLIME_GRAVITY;
	float SLIME_JUMP_SPEED;

	float VIEW_WIDTH;
	float VIEW_HEIGHT;
	float FLOOR_Y;
	float NET_WIDTH;
	float NET_HEIGHT;
	float NET_DEPTH;

	int SCORE_PAUSE_TIME;
	int MAX_SCORE;
	int MAX_PLAYERS;


	Vector2f SCORE_OFFSET;
	Vector2f SCORE_RECT;
	float POINT_RADIUS_SPACING_RATIO;
	float MAX_POINT_RADIUS;

	static const int NUM_SLIME_COLORS = 9;
	Color SLIME_COLORS[NUM_SLIME_COLORS];

};

#endif // _CONFIG_H_