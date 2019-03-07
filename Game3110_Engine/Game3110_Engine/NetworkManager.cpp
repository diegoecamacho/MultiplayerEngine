#include "NetworkManager.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include <iostream>

NetworkManager::NetworkManager()
	: m_maxConnections(1u)
	, m_hostIp("127.0.0.1")
	, m_hostPort(5555)
{
	m_rakPeerInterface = RakNet::RakPeerInterface::GetInstance();
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::Init()
{
	if (!m_isRunning)
	{
		m_isRunning = true;
		m_packetThread = new std::thread(&NetworkManager::Update, this);
	}

	//open up socket
	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily = AF_INET; // Test out IPV4

	while (RakNet::IRNS2_Berkley::IsPortInUse(socketDescriptor.port, socketDescriptor.hostAddress, socketDescriptor.socketFamily, SOCK_DGRAM) == true)
		socketDescriptor.port++;

	RakNet::StartupResult result = m_rakPeerInterface->Startup(m_maxConnections, &socketDescriptor, 1);
	assert(result == RakNet::StartupResult::RAKNET_STARTED);

	AttemptConnection();
}

void NetworkManager::AttemptConnection()
{
	RakNet::ConnectionAttemptResult connectionResult = m_rakPeerInterface->Connect(m_hostIp, m_hostPort, nullptr, 0);
	assert(connectionResult == RakNet::CONNECTION_ATTEMPT_STARTED);
	std::cout << "attempting to connect to " << m_hostIp << " Port: " << m_hostPort << std::endl;
}

void NetworkManager::Shutdown()
{
	m_isRunning = false;
	m_packetThread->join();
	delete m_packetThread;
}

void NetworkManager::PacketListener()
{
	RakNet::Packet* packet;
	for (packet = m_rakPeerInterface->Receive(); packet != nullptr; m_rakPeerInterface->DeallocatePacket(packet), packet = m_rakPeerInterface->Receive())
	{
		//received a packet
		for (auto &packetCallback : m_callbacks) // access by reference to avoid copying
		{
			packetCallback(packet);
		}
	}
}

void NetworkManager::Update()
{
	while (m_isRunning)
	{
		PacketListener();
	}
}

unsigned char NetworkManager::GetPacketIdentifier(RakNet::Packet *p)
{
	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char)p->data[0];
}

bool NetworkManager::SendPacket(const SendStruct& params)
{
	return m_rakPeerInterface->Send(&params.bitStream, params.priority, params.reliability, params.orderingChannel, params.systemIdentifier, params.broadcast);
}