#ifndef _CLIENT_APP_H_
#define _CLIENT_APP_H_

#include <RakNet/BitStream.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>
#include <RakNet/MessageIdentifiers.h>
#include <GameLib/Application.h>
#include <GameLib/graphics/Graphics.h>
#include <math/Vector2f.h>
#include <math/Rect2f.h>
#include "Config.h"
#include "Ball.h"
#include "Slime.h"
#include "Team.h"
#include "Messages.h"
#include <vector>
#include <map>

class ClientApp : public Application
{
public:
	ClientApp(RakNet::RakPeerInterface* peerInterface);
	~ClientApp();

	void ReadConnectionAcceptedPacket(RakNet::Packet* packet);

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

	typedef std::map<int, Slime*> PlayerMap;

	Texture* m_testTexture;
	Font* m_fontScore;
	Font* m_fontSmall;

	ColorScheme m_colorScheme;
	GameConfig m_gameConfig;

	RakNet::RakPeerInterface* m_peerInterface;
	float m_chooseColorButtonRadius;
	int m_selectedColorButtonIndex;
	std::vector<Vector2f> m_chooseColorButtons;
	std::vector<Rect2f> m_joinTeamButtons;
	int m_state;
	Ball m_ball;
	Team m_teams[2];
	Slime* m_player;
	PlayerMap m_players;
	RakNet::RakNetGUID m_serverGuid;
};


#endif // _CLIENT_APP_H_