#pragma once
#include "InputManager.h"
#include "NetworkManager.h"
#include "PlayerStats.h"
#include "GameStates.h"

class RPGGame
{
public:

	bool m_isRunning = false;
private:
	InputManager* m_inputManager;
	NetworkManager* m_networkManager;

	GameStates m_currentState = S_Default;

	bool m_inStatsMenu = false;
	bool m_playerAttacking = false;
	bool m_statsAvailable = false;

	PlayerStats m_clientPlayerStats;
	std::vector<PlayerStats> availableEnemies;
public:
	RPGGame() = default;
	~RPGGame() = default;

	void Init();

	//Callback Setup
	void SetupInputListener(InputManager* inputManager);
	void SetupNetworkListener(NetworkManager* networkManager);

	void Update();

	void WinConditionMet();

	void PrintAttackMissed();

	//Callback Handles
	void HandlePackets(RakNet::Packet* packet);

	void LoseCondition();

	void ReceivePlayerTakeDamage(RakNet::Packet* packet);

	void HandleInput(std::string userInput);

private:
	//Menu Functions
	void PrintPlayerNameRequest();
	void PrintPlayerClassRequest();
	void PrintPlayerStats();
	void PrintWelcomeText();
	void PrintPlayerTurn();
	void PrintGameBegin();
	void PrintWaitingPlayerTurn();
	void PrintDefenceMenu();
	void PrintAttackMenu();
	void PrintAllPlayerStats();

	void PrintEnemyPlayers();

	//Input Handler Functions
	void SetPlayerName(std::string playerName);
	void SetPlayerClass(std::string playerClass);
	void NavigateMenu(std::string userInput);
	void PlayerAttackChoice(std::string userInput);

	//Network Sending Functions
	void SendPlayerAttack(PlayerStats& player);

	void SendPlayerClass(std::string playerClass);
	void SendStatsRequest();

	void ReceiveLatestPlayerInfo(RakNet::Packet* packet);

};
