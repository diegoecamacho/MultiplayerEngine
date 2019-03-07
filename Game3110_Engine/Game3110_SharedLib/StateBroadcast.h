#pragma once
#include "RakNetTypes.h"
#include "SendStruct.h"

static void SendStateChangePacket(SendStruct& outStruct, RakNet::MessageID msgID) {
	outStruct.bitStream.Write(msgID);
}

static void SendStateChangePacket(SendStruct& outStruct, RakNet::MessageID msgID, RakNet::RakNetGUID systemAddress, bool broadcast = false) {
	outStruct.broadcast = broadcast;
	outStruct.systemIdentifier = systemAddress;
	outStruct.bitStream.Write(msgID);
}
