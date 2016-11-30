#ifndef _CLIENT_APP_H_
#define _CLIENT_APP_H_

#include <RakNet/BitStream.h>
#include <RakNet/MessageIdentifiers.h>
#include <RakNet/RakNetTypes.h>  // MessageID
#include <RakNet/RakPeerInterface.h>

#include <map>
#include <vector>

#include <GameLib/Application.h>
#include <GameLib/graphics/Graphics.h>
#include <GameLib/math/Rect2f.h>
#include <GameLib/math/Vector2f.h>

#include "Ball.h"
#include "Config.h"
#include "Messages.h"
#include "NetworkManagerClient.h"
#include "Slime.h"
#include "Team.h"
#include "GameWorld.h"


class ClientApp : public Application
{
public:
	ClientApp(RakNet::RakPeerInterface* peerInterface);
	~ClientApp();

	void ReadConnectionAcceptedPacket(RakNet::Packet* packet);

	float GetTimeStamp();

protected:
	void OnInitialize();
	void OnUpdate(float timeDelta);
	void OnRender();

private:

	void ReceivePackets();
	void ReceivePacketPlayerJoined(RakNet::BitStream& inStream);
	void ReceivePacketPlayerLeft(RakNet::BitStream& inStream);
	void ReceivePacketTeamScored(RakNet::BitStream& inStream);
	void ReceivePacketTeamServe(RakNet::BitStream& inStream);
	void ReceivePacketWaitingForPlayers(RakNet::BitStream& inStream);
	void ReceivePacketUpdateTick(RakNet::BitStream& inStream);

	void DrawSlime(Graphics& g, const Slime& slime, const Vector2f& lookAtPoint);

	enum
	{
		CLIENT_STATE_CHOOSE_COLOR = 0,
		CLIENT_STATE_CHOOSE_TEAM,
		CLIENT_STATE_PLAY_GAME,
		CLIENT_STATE_WAIT_FOR_SERVE,
	};

	typedef std::map<PlayerID, Slime*> PlayerMap;

	// Resources
	Texture*	m_testTexture;
	Font*		m_fontScore;
	Font*		m_fontSmall;
	
	// Client game config
	ColorScheme				m_colorScheme;
	float					m_chooseColorButtonRadius;
	int						m_selectedColorButtonIndex;
	std::vector<Vector2f>	m_chooseColorButtons;
	std::vector<Rect2f>		m_joinTeamButtons;

	// Client game state
	GameWorld	m_gameWorld;
	int			m_state;
	Slime*		m_player;

	// Managers
	RakNet::RakPeerInterface*	m_peerInterface;
	InputManager				m_inputManager;
	NetworkManagerClient		m_networkManager;
};


#endif // _CLIENT_APP_H_