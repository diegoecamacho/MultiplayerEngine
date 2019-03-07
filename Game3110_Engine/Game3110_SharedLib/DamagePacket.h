#pragma once
#include <string>
#include "SendStruct.h"

class DamagePacket {
public:
	int damageDone = 0;

	void SerializePacket(SendStruct& sendPacket, RakNet::MessageID ID) {
		sendPacket.bitStream.Write(ID);
		sendPacket.bitStream.Write(damageDone);
	}

	void DeserializePacket(RakNet::Packet* packet) {
		RakNet::BitStream bitstream(packet->data, packet->length, false);
		bitstream.IgnoreBytes(sizeof(RakNet::MessageID));
		bitstream.Read(damageDone);

	}
};

