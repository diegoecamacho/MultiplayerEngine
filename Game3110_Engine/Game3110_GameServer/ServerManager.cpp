#include "ServerManager.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include <stdlib.h>

ServerManager::ServerManager() : m_hostIp("127.0.0.1"), m_hostPort(5555), m_maxConnections(4u),
m_isRunning(false), m_serverListener(nullptr)
{
	m_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
}

ServerManager::~ServerManager()
{
	Shutdown();
}

void ServerManager::Init()
{
	if (!m_isRunning)
	{
		m_isRunning = true;
		m_serverListener = new std::thread(&ServerManager::PacketListener, this);
	}

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.port = m_hostPort;
	socketDescriptor.socketFamily = AF_INET; // Test out IPV4

	RakNet::StartupResult result = m_rakPeerInterface->Startup(m_maxConnections, &socketDescriptor, 1);
	assert(result == RakNet::StartupResult::RAKNET_STARTED);

	m_rakPeerInterface->SetMaximumIncomingConnections(m_maxConnections);
}

void ServerManager::Shutdown()
{
	m_isRunning = false;
	m_serverListener->join();
	delete m_serverListener;
}

void ServerManager::PacketListener()
{
	while (m_isRunning)
	{
		RakNet::Packet* packet;
		for (packet = m_rakPeerInterface->Receive(); packet != nullptr; m_rakPeerInterface->DeallocatePacket(packet), packet = m_rakPeerInterface->Receive())
		{
			RakNet::MessageID msgID = GetPacketIdentifier(packet);
			//received a packet
			for (auto &packetCallback : m_callbacks) // access by reference to avoid copying
			{
				packetCallback(packet);
			}
		}
	}
}

bool ServerManager::SendPacket(const SendStruct& params)
{
	return m_rakPeerInterface->Send(&params.bitStream, params.priority, params.reliability, params.orderingChannel, params.systemIdentifier, params.broadcast);
}

unsigned char ServerManager::GetPacketIdentifier(RakNet::Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char)p->data[0];
}