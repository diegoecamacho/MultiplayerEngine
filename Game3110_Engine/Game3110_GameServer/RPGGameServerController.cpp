#include <stdlib.h>
#include <iostream>
#include "RPGGameServerController.h"
#include "MessageIdentifiers.h"
#include "PlayerStats.h"
#include "RakString.h"
#include "StateBroadcast.h"
#include "DamagePacket.h"

#pragma region Initialization

void RPGGameServerController::Init()
{
	m_isRunning = true;
	std::cout << "System Init \n";
	m_currentState = WaitingForPlayers;
}

void RPGGameServerController::SetupServerListener(ServerManager* serverManager)
{
	if (!serverManager)
	{
		return;
	}
	m_serverManager = serverManager;
	serverManager->AddCallback(this, &RPGGameServerController::HandlePackets);
}

#pragma endregion

#pragma region Update

void RPGGameServerController::Update()
{
	switch (m_currentState)
	{
	case RPGGameServerController::WaitingForPlayers:
		WaitingForPlayerLobby();
		break;
	case RPGGameServerController::PlayerTurn:
		GettingNextPlayer();
		break;
	case RPGGameServerController::WaitingPlayer:
		GameOverCheck();

		return;
	default:
		break;
	}
}

void RPGGameServerController::GameOverCheck()
{

	for (int i = 0; i <= (activePlayers - 1); i++)
	{
		if (connectedPlayers[i].m_Health <= 0)
		{
			SendStruct sendParam;

			SendStateChangePacket(sendParam, ID_GAME_OVER, connectedPlayers[i].m_SystemAddress, false);

			m_serverManager->SendPacket(sendParam);
			connectedPlayers.erase(connectedPlayers.begin() + i);
			activePlayers--;
		}

	}
}

void RPGGameServerController::HandlePackets(RakNet::Packet* packet)
{
	if (!HandleDefaultPackets(packet))
	{
		RakNet::MessageID msgID = ServerManager::GetPacketIdentifier(packet);
		switch (msgID)
		{
		case ID_PLAYER_ADDED:
			ReceivePlayerAdded(packet);
			break;
		case  ID_ATTACK_COMMAND:
			PlayerAttacking(packet);
			break;
		case ID_DEFENCE_COMMAND:
			ReceivePlayerDefend(packet);
			break;
		case ID_STATS_COMMAND:
			ReceiveUpdateStats(packet);
			break;
		case ID_MISSED_ATTACK:
			m_currentState = PlayerTurn;
			break;
		default:
			break;
		}
	}
}

bool RPGGameServerController::HandleDefaultPackets(RakNet::Packet* packet)
{
	bool isHandled = true;

	RakNet::MessageID msgId = ServerManager::GetPacketIdentifier(packet);
	switch (msgId)
	{
	case ID_CONNECTION_REQUEST_ACCEPTED:
		std::cout << "ID_CONNECTION_REQUEST_ACCEPTED" << std::endl;
		break;
	case ID_NEW_INCOMING_CONNECTION:
		std::cout << "ID_NEW_INCOMING_CONNECTION" << std::endl;
		std::cout << "Packet Address " << packet->systemAddress.ToString() << std::endl;
		//g_peerInterface->SetTimeoutTime(1000, packet->systemAddress);
		break;
	case ID_CONNECTION_ATTEMPT_FAILED:
		std::cout << "ID_CONNECTION_ATTEMPT_FAILED" << std::endl;
		break;
	case ID_CONNECTION_LOST:
		std::cout << "ID_CONNECTION_LOST" << std::endl;
		break;
	default:
		isHandled = false;
		break;
	}
	return isHandled;
}

#pragma endregion

#pragma region ServerStates

void RPGGameServerController::WaitingForPlayerLobby()
{
	if (connectedPlayers.size() >= 3) {
		SendStruct gameStartBroadcast;
		SendStateChangePacket(gameStartBroadcast, ID_GAME_BEGIN);
		m_serverManager->SendPacket(gameStartBroadcast);
		m_currentState = PlayerTurn;
	}
}

void RPGGameServerController::GettingNextPlayer()
{
	SendStruct turnBroadcast;
	SendStateChangePacket(turnBroadcast, ID_PLAYER_TURN, connectedPlayers[currentTurn].m_SystemAddress);
	SendStruct waitBroadcast;
	SendStateChangePacket(waitBroadcast, ID_WAIT_FOR_TURN, connectedPlayers[currentTurn].m_SystemAddress, true);
	m_serverManager->SendPacket(turnBroadcast);
	m_serverManager->SendPacket(waitBroadcast);

	currentTurn++;
	if (currentTurn >= activePlayers)
	{
		currentTurn = 0;
	}

	m_currentState = WaitingPlayer;
}

void RPGGameServerController::PlayerAttacking(RakNet::Packet* packet)
{
	PlayerStats playerHurt;
	playerHurt.DeserializePacket(packet);
	playerHurt.PrintStats();

	for (int i = 0; i <= (activePlayers - 1); i++)
	{
		if (connectedPlayers[i].GetName() == playerHurt.GetName())
		{
			if (connectedPlayers[i].invulnerable)
			{
				SendStruct failedpacket;
				connectedPlayers[i].invulnerable = false;
				SendStateChangePacket(failedpacket, ID_MISSED_ATTACK);
				m_serverManager->SendPacket(failedpacket);
			}
			else {
				connectedPlayers[i].TakeDamage(50);
				SendDamagePacket(connectedPlayers[i]);
			}
		}
	}

	m_currentState = PlayerTurn;
}

#pragma endregion

#pragma region PacketSending

void RPGGameServerController::SendDamagePacket(PlayerStats& player)
{
	SendStruct damagePacket;
	damagePacket.broadcast = false;
	damagePacket.systemIdentifier = player.m_SystemAddress;
	player.SerializePacket(damagePacket, true, ID_DAMAGE_DONE);
	

	SendStruct broadcastPacket;
	SendStateChangePacket(broadcastPacket, ID_STATS_RECEIVED, player.m_SystemAddress, true);

	m_serverManager->SendPacket(damagePacket);
	m_serverManager->SendPacket(broadcastPacket);

	m_currentState = PlayerTurn;
}

void RPGGameServerController::ReceivePlayerDefend(RakNet::Packet* packet)
{
	PlayerStats playerDefending;
	playerDefending.DeserializePacket(packet);
	
	for (int i = 0; i <= (activePlayers - 1); i++)
	{
		if (connectedPlayers[i].GetName() == playerDefending.GetName())
		{
			connectedPlayers[i].invulnerable = true;
			std::cout << connectedPlayers[i].m_Name << "\n";
			break;
		}
	}
	m_currentState = PlayerTurn;
}

void RPGGameServerController::ReceivePlayerAdded(RakNet::Packet* packet)
{
	std::cout << "\nClass Received" << std::endl;

	PlayerStats player;
	player.DeserializePacket(packet);

	std::cout << "Player Added : \n";
	player.PrintStats();

	connectedPlayers.push_back(player);
	activePlayers++;
}

void RPGGameServerController::ReceiveUpdateStats(RakNet::Packet* packet)
{
	std::cout << "Requesting Stats \n";
	SendStruct statsPacket;
	statsPacket.broadcast = false;
	statsPacket.systemIdentifier = packet->guid;
	statsPacket.bitStream.Write((RakNet::MessageID)ID_STATS_RECEIVED);
	statsPacket.bitStream.Write(connectedPlayers.size());

	for (auto& player : connectedPlayers) {
		player.SerializePacket(statsPacket, false);
	}

	m_serverManager->SendPacket(statsPacket);
}

#pragma endregion