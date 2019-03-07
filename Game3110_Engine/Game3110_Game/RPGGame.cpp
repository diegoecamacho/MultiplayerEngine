#include <mutex>
#include <iostream>
#include "RPGGame.h"
#include "MessageIdentifiers.h"
#include "StateBroadcast.h"
#include "GameStates.h"
#include "BitStream.h"
#include "DamagePacket.h"

#pragma region Initialization
//Init
void RPGGame::SetupInputListener(InputManager* inputManager)
{
	if (!inputManager)
	{
		return;
	}
	m_inputManager = inputManager;
	inputManager->AddCallback(this, &RPGGame::HandleInput);
}

void RPGGame::SetupNetworkListener(NetworkManager* networkManager)
{
	if (!networkManager)
	{
		return;
	}
	m_networkManager = networkManager;
	networkManager->AddCallback(this, &RPGGame::HandlePackets);
}

void RPGGame::Init()
{
	m_isRunning = true;
}
#pragma endregion Initialization

#pragma region Update
// Update
void RPGGame::Update()
{
	switch (m_currentState)
	{
	case S_PrintPlayerName:
		PrintPlayerNameRequest();
		break;
	case S_PrintClassQuestion:
		PrintPlayerClassRequest();
		break;
	case S_PrintWelcomeMessage:
		PrintWelcomeText();
		break;
	case S_GameBegin:
		PrintGameBegin();
		break;
	case S_WaitingForTurn:
		PrintWaitingPlayerTurn();
		break;
	case S_PrintPlayerTurnMenu:
		PrintPlayerTurn();
		break;
	case S_PlayerAttack:
		PrintAttackMenu();
		break;
	case S_AttackMissed:
		PrintAttackMissed();
		break;
	case S_PlayerDefend:
		PrintDefenceMenu();
		break;
	case S_SendStatsRequest:
		SendStatsRequest();
		break;
	case S_PrintStatsRequest:
		PrintAllPlayerStats();
		break;
	case S_YouWin:
		WinConditionMet();

		break;
	default:
		break;
	}
}

void RPGGame::WinConditionMet()
{
	std::cout << " You Win! \n";
	m_isRunning = false;
}

void RPGGame::HandlePackets(RakNet::Packet* packet)
{
	RakNet::MessageID msgID = NetworkManager::GetPacketIdentifier(packet);
	switch (msgID)
	{
	case ID_CONNECTION_REQUEST_ACCEPTED:
		std::cout << "Connection Request Accepted" << std::endl;
		m_currentState = S_PrintPlayerName;
		break;
	case ID_GAME_BEGIN:
		m_currentState = S_GameBegin;
		break;
	case ID_PLAYER_TURN:
		system("CLS");
		m_currentState = S_PrintPlayerTurnMenu;
		break;
	case ID_WAIT_FOR_TURN:
		m_currentState = S_WaitingForTurn;
		break;

	case ID_DAMAGE_DONE:
		ReceivePlayerTakeDamage(packet);
		break;
	case ID_MISSED_ATTACK:
		PrintAttackMissed();
		break;
	case ID_STATS_RECEIVED:
		ReceiveLatestPlayerInfo(packet);
		break;

	case ID_GAME_OVER:
		LoseCondition();

		break;
	default:
		std::cout << "Message ID:" << msgID << std::endl;
		break;
	}
}

void RPGGame::LoseCondition()
{
	std::cout << "\nGame Over! You lose! \n";
	m_isRunning = false;
}

void RPGGame::HandleInput(std::string userInput)
{
	switch (m_currentState)
	{
	case S_NameInput:
		SetPlayerName(userInput);
		break;
	case S_ClassInput:
		SetPlayerClass(userInput);
		break;
	case S_PlayerTurnInput:
		NavigateMenu(userInput);
		break;
	case S_AttackChoice:
		PlayerAttackChoice(userInput);
		break;
	default:
		break;
	}
}
#pragma endregion Update

#pragma region PlayerInput
void RPGGame::SetPlayerName(std::string playerName)
{
	m_clientPlayerStats.SetName(playerName);
	m_currentState = S_PrintClassQuestion;
}

void RPGGame::SetPlayerClass(std::string playerClass)
{
	char classChar = playerClass[0];
	switch (classChar)
	{
	case 'W': {
		SendPlayerClass("Warrior");
		break;
	}
	case 'T': {
		SendPlayerClass("Thief");
		break;
	}
	case 'M': {
		SendPlayerClass("Mage");
		break;
	}
	default:
	{
		std::cout << "Sorry That is not a valid class, Please try again!" << std::endl;
		m_currentState = S_PrintClassQuestion;
		break;
	}
	}
}

void RPGGame::NavigateMenu(std::string userInput)
{
	char input = userInput[0];
	switch (input)
	{
	case 'S':
		m_inStatsMenu = true;
		m_currentState = S_SendStatsRequest;
		break;
	case 'A':
		m_currentState = S_PlayerAttack;
		break;
	case 'D':
		m_currentState = S_PlayerDefend;
		break;
	default:
		std::cout << "Please Enter a correct menu option! \n";
		m_currentState = S_PrintPlayerTurnMenu;
		break;
	}
}

void RPGGame::PlayerAttackChoice(std::string userInput)
{
	for (int x = 0; x < availableEnemies.size(); x++)
	{
		if (availableEnemies[x].GetName() == userInput)
		{
			SendPlayerAttack(availableEnemies[x]);
			m_currentState = S_Default;
			break;
		}
	}
	m_playerAttacking = false;
	
}
#pragma endregion PlayerInput

#pragma region PrintStatements
//Prints
void RPGGame::PrintPlayerNameRequest()
{
	system("CLS");
	std::cout << "=================================== \n";
	std::cout << "Please Enter your Name" << std::endl;
	std::cout << "=================================== \n";
	m_currentState = S_NameInput;
}

void RPGGame::PrintPlayerClassRequest()
{
	system("CLS");
	std::cout << "What class would you like to be?" << std::endl;
	std::cout << "=================================== \n";
	std::cout << "Warrior [W] | Thief [T] | Mage [M]" << std::endl;
	std::cout << "=================================== \n";

	m_currentState = S_ClassInput;
}

void RPGGame::PrintPlayerStats()
{
	m_clientPlayerStats.PrintStats();
}

void RPGGame::PrintWelcomeText()
{
	std::cout << "=================================== \n";
	std::cout << "Welcome To Magic Colosseum!!! \n\n";
	std::cout << "Player Cards :  \n\n";
	m_clientPlayerStats.PrintStats();

	std::cout << "\n\n";
	std::cout << "=================================== \n";
	std::cout << "\nWaiting on Other Players... \n";
	std::cout << "=================================== \n";

	m_currentState = S_WaitingForStart;
}

void RPGGame::PrintPlayerTurn()
{
	std::cout << "What would you like to do? \n";
	std::cout << "=================================== \n";
	std::cout << "Stats[S] | Attack [A] | Defend[D] \n";
	std::cout << "=================================== \n";
	m_currentState = S_PlayerTurnInput;
}

void RPGGame::PrintGameBegin()
{
	system("CLS");
	std::cout << "Game Begin! \n\n";
}

void RPGGame::PrintAttackMenu()
{
	SendStatsRequest();

	system("CLS");
	std::cout << "Who do you want to attack? \n";
	std::cout << "=================================== \n";

	m_playerAttacking = true;
	m_currentState = S_PrintStatsRequest;
}

void RPGGame::PrintDefenceMenu()
{
	system("CLS");
	std::cout << "=================================== \n";
	std::cout << "You are Invulnerable for this Turn! \n";
	std::cout << "=================================== \n";

	SendStruct sendStruct;
	m_clientPlayerStats.SerializePacket(sendStruct, true, ID_DEFENCE_COMMAND);
	m_networkManager->SendPacket(sendStruct);

	m_currentState = S_Default;
}

void RPGGame::PrintAllPlayerStats()
{
	if (m_statsAvailable)
	{
		if (!m_playerAttacking)
		{
			std::cout << "=================================== \n";
			std::cout << "Player Stats: \n";
			m_clientPlayerStats.PrintStats();
			std::cout << "=================================== \n";
		}

		PrintEnemyPlayers();

		if (m_playerAttacking)
		{
			std::cout << "Who would you like to attack? \n";
			m_currentState = S_AttackChoice;
		}
		else
		{
			m_currentState = S_PrintPlayerTurnMenu;
		}
	}
}

void RPGGame::PrintEnemyPlayers()
{
	if (m_statsAvailable)
	{
		for (auto& enemyPlayers : availableEnemies)
		{
			std::cout << "=================================== \n";
			enemyPlayers.PrintStats();
			std::cout << "=================================== \n";
		}
	}
}

void RPGGame::PrintWaitingPlayerTurn()
{
	std::cout << "Waiting for Turn...";
	m_currentState = S_Default;
}

void RPGGame::PrintAttackMissed()
{
	std::cout << "\n=================================== \n";
	std::cout << "Attack Missed! \n";


	m_currentState = S_Default;
}

#pragma endregion PrintStatements

#pragma region NetworkHandlers
//Sets Players class and sends and instance to the server
void RPGGame::SendPlayerClass(std::string playerClass)
{
	m_clientPlayerStats.m_Class = playerClass;

	SendStruct sendPacket;
	m_clientPlayerStats.SerializePacket(sendPacket, true, ID_PLAYER_ADDED);

	m_networkManager->SendPacket(sendPacket);

	m_currentState = S_PrintWelcomeMessage;
}

void RPGGame::SendPlayerAttack(PlayerStats& player)
{
	SendStruct sendPacket;

	player.SerializePacket(sendPacket, true, ID_ATTACK_COMMAND);

	m_networkManager->SendPacket(sendPacket);

	m_playerAttacking = false;
}

void RPGGame::SendStatsRequest()
{
	system("CLS");
	SendStruct sendStruct;
	SendStateChangePacket(sendStruct, ID_STATS_COMMAND);
	m_networkManager->SendPacket(sendStruct);

	m_statsAvailable = false;
	m_currentState = S_Default;
}

void RPGGame::ReceivePlayerTakeDamage(RakNet::Packet* packet)
{
	m_clientPlayerStats.DeserializePacket(packet);

	m_currentState = S_Default;
}

void RPGGame::ReceiveLatestPlayerInfo(RakNet::Packet* packet)
{
	availableEnemies.clear();

	RakNet::BitStream bitstream(packet->data, packet->length, false);
	bitstream.IgnoreBytes(sizeof(RakNet::MessageID));
	int playerSize;
	bitstream.Read(playerSize);

	for (int x = 0; x < playerSize; x++)
	{
		PlayerStats player;
		player.DeserializePacket(bitstream, packet);
		if (player.GetName() == m_clientPlayerStats.GetName()) {
			continue;
		}
		else {
			availableEnemies.push_back(player);
		}
	}
	m_statsAvailable = true;

	if (m_inStatsMenu)
	{
		m_inStatsMenu = false;
		m_currentState = S_PrintStatsRequest;
	}
	else
	{
		m_currentState = S_Default;
	}
}
#pragma endregion NetworkHandlers