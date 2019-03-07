#include "InputManager.h"
#include <iostream>

InputManager::InputManager() : m_isRunning(false), m_listenerThread(nullptr)
{
}

InputManager::~InputManager()
{
	Shutdown();
}

void InputManager::Init()
{
	if (!m_isRunning)
	{
		m_isRunning = true;
		m_listenerThread = new std::thread(&InputManager::InputListener, this);
	}
}

void InputManager::Shutdown()
{
	m_isRunning = false;
	m_listenerThread->join();
	delete m_listenerThread;
}

void InputManager::InputListener()
{
	std::string userInput;
	while (m_isRunning)
	{
		std::getline(std::cin, userInput);
		//received a packet
		for (auto &inputCallback : m_callbacks) // access by reference to avoid copying
		{
			inputCallback(userInput);
		}
	}
}