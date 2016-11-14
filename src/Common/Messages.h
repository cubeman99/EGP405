#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <RakNet/MessageIdentifiers.h>


struct PacketType
{
	enum
	{
		UNKNOWN = ID_USER_PACKET_ENUM + 1,

		// Server --> Client 
		ACCEPT_CONNECTION,
		PLAYER_JOINED,
		PLAYER_LEFT,
		TEAM_SCORED,
		TEAM_SERVE,
		UPDATE_TICK,

		// Client --> Server
		DISCONNECTED,
		CLIENT_UPDATE_TICK,
		
		EVENTS_BEGIN,
			JOIN_TEAM,
		EVENTS_END,

		INPUT,
	};
};

#endif // _MESSAGES_H_