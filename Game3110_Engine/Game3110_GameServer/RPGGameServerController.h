#pragma once
#include "ServerManager.h"
#include <vector>
#include "PlayerStats.h"

class RPGGameServerController
{
public:
	bool m_isRunning = false;

	enum ServerStates
	{
		Default,
		WaitingForPlayers,
		PlayerTurn,
		WaitingPlayer
	};

private:
	ServerStates m_currentState;
	ServerManager* m_serverManager;
	std::vector<PlayerStats> connectedPlayers;

	int activePlayers = 0;
	int currentTurn = 0;
public:
	RPGGameServerController() = default;

	~RPGGameServerController() = default;

	void Init();

	void Update();

	void GameOverCheck();

	void WaitingForPlayerLobby();

	void GettingNextPlayer();


	void SetupServerListener(ServerManager* serverManager);

	void HandlePackets(RakNet::Packet* packet);

	void ReceiveUpdateStats(RakNet::Packet* packet);

	void PlayerAttacking(RakNet::Packet* packet);

	void SendDamagePacket(PlayerStats &player);

	void ReceivePlayerDefend(RakNet::Packet* packet);

	bool HandleDefaultPackets(RakNet::Packet* packet);

	//Packet Handling Functions
	void ReceivePlayerAdded(RakNet::Packet* packet);
};
