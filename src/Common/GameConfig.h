#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <math/Vector2f.h>
#include <vector>

// Configuration values for a game of Slime Volleyball.

#pragma pack(1)
struct GameConfig
{
	GameConfig() {}

	struct
	{
		float radius;
		float gravity;
		float maxSpeed;
		float serveHeight;
		float maxBounceXVelocity;
		float maxBounceYVelocity;
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
#pragma pack()


#endif // _CONFIG_H_