#pragma once
#include "PacketPriority.h"
#include "BitStream.h"
#include "RakNetTypes.h"

struct SendStruct
{
public:
	SendStruct()
		: priority(PacketPriority::HIGH_PRIORITY)
		, reliability(PacketReliability::RELIABLE)
		, orderingChannel(0)
		, broadcast(true)
	{
	}

	~SendStruct() {}

	RakNet::BitStream bitStream;
	PacketPriority priority;
	PacketReliability reliability;
	char orderingChannel;
	RakNet::RakNetGUID systemIdentifier;
	bool broadcast;
};
