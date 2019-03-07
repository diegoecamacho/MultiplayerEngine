#include "stdafx.h"
#include "PlayerStats.h"
#include "BitStream.h"
#include <iostream>
#include "RakString.h"
#include "SendStruct.h"

PlayerStats::PlayerStats(std::string playerName) : m_Name(playerName)
{
}

void PlayerStats::PrintStats()
{
	std::cout << "Name: " << GetName() << std::endl;
	std::cout << "Class: " << m_Class << std::endl;
	std::cout << "Health: " << m_Health << std::endl << std::endl;
}

void PlayerStats::TakeDamage(int damage)
{
	m_Health -= damage;
}

void PlayerStats::SerializePacket(SendStruct& outStruct, bool includeMsgID, RakNet::MessageID msgID)
{
	if (includeMsgID)
	{
		outStruct.bitStream.Write(msgID);
	}

	outStruct.bitStream.Write(RakNet::RakString(m_Name.c_str()));
	outStruct.bitStream.Write(RakNet::RakString(m_Class.c_str()));
	outStruct.bitStream.Write(m_Health);
	outStruct.bitStream.Write(invulnerable);
}

void PlayerStats::DeserializePacket(const RakNet::Packet* packet)
{
	RakNet::BitStream bitstream(packet->data, packet->length, false);

	RakNet::MessageID msgID;
	bitstream.Read(msgID);

	int health;
	bool invul;
	RakNet::RakString name, playerClass;

	bitstream.Read(name);
	bitstream.Read(playerClass);
	bitstream.Read(health);
	bitstream.Read(invul);

	m_Name = name;
	m_Class = playerClass;
	m_Health = health;
	invulnerable = invul;

	m_SystemAddress = packet->guid;
}

void PlayerStats::DeserializePacket(RakNet::BitStream& bitstream,const RakNet::Packet* packet)
{
	int health;
	bool invul;
	RakNet::RakString name, playerClass;

	bitstream.Read(name);
	bitstream.Read(playerClass);
	bitstream.Read(health);
	bitstream.Read(invul);

	m_Name = name;
	m_Class = playerClass;
	m_Health = health;
	invulnerable = invul;

	if (packet != nullptr)
	{
		m_SystemAddress = packet->guid;
	}
}

bool PlayerStats::operator==(PlayerStats rhs)
{
	return (m_Name == rhs.m_Name);
}
