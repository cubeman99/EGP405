#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <RakNet/MessageIdentifiers.h>



struct GameMessages
{
	enum
	{
		UNKNOWN = ID_USER_PACKET_ENUM + 1,

		// Server --> Client 
		ACCEPT_CONNECTION,
		PLAYER_JOINED,
		PLAYER_LEFT,
		PLAYER_SCORED,
		ROUND_ENDED,
		TEAM_SERVE,
		UPDATE_TICK,

		// Client --> Server
		DISCONNECTED,
		JOIN_GAME,
		CLIENT_UPDATE_TICK,
	};
};


#endif // _MESSAGES_H_