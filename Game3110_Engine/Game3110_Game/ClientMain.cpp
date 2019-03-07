#include "InputManager.h"
#include "RPGGame.h"

int main() {
	bool continueGame = true;

	InputManager inputManager;
	inputManager.Init();

	NetworkManager networkManager;
	networkManager.Init();

	RPGGame gameInstance;

	gameInstance.Init();
	gameInstance.SetupInputListener(&inputManager);
	gameInstance.SetupNetworkListener(&networkManager);

	while (continueGame)
	{
		gameInstance.Update();
		continueGame = gameInstance.m_isRunning;
	}
}