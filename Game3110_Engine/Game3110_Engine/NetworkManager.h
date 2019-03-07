#pragma once

#include "RakNetTypes.h"
#include "PacketPriority.h"
#include "BitStream.h"
#include "SendStruct.h"

#include <thread>
#include <vector>

//forward declaring rakpeerInterface
//don't need definition in header
namespace RakNet
{
	class RakPeerInterface;
	class Packet;
}

class NetworkManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Init();
	void AttemptConnection();
	void Shutdown();

	bool SendPacket(const SendStruct& params);

	static unsigned char GetPacketIdentifier(RakNet::Packet *p);

	template<class T>
	void AddCallback(T* const object, void(T::* memberFunction)(RakNet::Packet*))
	{
		using namespace std::placeholders;
		m_callbacks.emplace_back(std::bind(memberFunction, object, _1));
	}

	void AddCallback(void(*const function)(RakNet::Packet*))
	{
		m_callbacks.emplace_back(function);
	}

	using PacketCallback = std::function<void(RakNet::Packet*)>;
	std::vector<PacketCallback> m_callbacks;
protected:
private:
	unsigned int m_maxConnections;
	const char* m_hostIp;
	unsigned int m_hostPort;
	RakNet::RakPeerInterface* m_rakPeerInterface;

	bool m_isRunning = false;
	std::thread* m_packetThread;


	void PacketListener();

	void Update();

};
