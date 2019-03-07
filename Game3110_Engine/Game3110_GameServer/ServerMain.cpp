#include <thread>
#include <iostream>

#include "RakNetTypes.h"
#include "ServerManager.h"
#include "RPGGameServerController.h"
#include "MessageIdentifiers.h"

int main() {

	bool continueRunning = true;

	ServerManager serverManager;
	serverManager.Init();

	RPGGameServerController serverController;
	serverController.Init();

	serverController.SetupServerListener(&serverManager);

	while (continueRunning)
	{
		serverController.Update();
		continueRunning = serverController.m_isRunning;
	}
}