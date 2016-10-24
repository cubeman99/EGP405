#ifndef _CLIENT_APP_H_
#define _CLIENT_APP_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <GameLib/Application.h>
#include <GameLib/graphics/Graphics.h>
#include <Common/Config.h>
#include <Common/Ball.h>
#include <Common/Slime.h>
#include <math/Vector2f.h>
#include <math/Rect2f.h>
#include <vector>
#include <map>
#include "Messages.h"

class ClientApp : public Application
{
public:
	ClientApp(RakNet::RakPeerInterface* peerInterface);
	~ClientApp();

	void ReadConnectionAcceptedPacket(RakNet::Packet* packet);

	inline const Config& GetConfig() const { return m_config; }

protected:
	void OnInitialize();
	void OnUpdate(float timeDelta);
	void OnRender();

private:
	void UpdatePlayer();
	void ReceivePackets();

	void DrawSlime(Graphics& g, const Slime& slime, const Vector2f& lookAtPoint);

	enum
	{
		STATE_CHOOSE_COLOR = 0,
		STATE_CHOOSE_TEAM,
		STATE_PLAY_GAME,
		STATE_WAIT_FOR_SERVE,
	};

	RakNet::RakPeerInterface* m_peerInterface;

	float m_chooseColorButtonRadius;
	int m_selectedColorButtonIndex;
	std::vector<Vector2f> m_chooseColorButtons;
	std::vector<Rect2f> m_joinTeamButtons;

	int m_state;
	Config m_config;
	Ball m_ball;

	Slime* m_player;



	typedef std::map<int, Slime*> PlayerMap;

	PlayerMap m_players;

	RakNet::RakNetGUID m_serverGuid;
};


#endif // _CLIENT_APP_H_