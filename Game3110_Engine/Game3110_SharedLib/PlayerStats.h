#pragma once
#include <string>
#include "RakNetTypes.h"
#include "SendStruct.h"
#include "MessageIdentifiers.h"

class PlayerStats
{
public:

	std::string m_Class = "";
	std::string m_Name = "";
	int m_Health = 100;
	bool invulnerable = false;

	RakNet::RakNetGUID m_SystemAddress;
private:

public:
	PlayerStats() = default;
	PlayerStats(std::string playerName);

	void SetName(std::string name) { m_Name = name; }
	std::string GetName() const { return m_Name; }

	void PrintStats();

	void TakeDamage(int damage);

	void SerializePacket(SendStruct& outStruct, bool includeMsgID = true, RakNet::MessageID msgID = ID_USER_PACKET_ENUM);
	void DeserializePacket(const RakNet::Packet* packet);
	void DeserializePacket(RakNet::BitStream& bitstream, const RakNet::Packet* packet = nullptr);

	bool operator==(PlayerStats rhs);

	~PlayerStats() = default;
};
