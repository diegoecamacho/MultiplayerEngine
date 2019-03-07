#pragma once

#pragma once

#include <string>
#include <thread>
#include <vector>

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();
	void Shutdown();

	void InputListener();

	template<class T>
	void AddCallback(T* const object, void(T::* memberFunction)(std::string inputLine))
	{
		using namespace std::placeholders;
		m_callbacks.emplace_back(std::bind(memberFunction, object, _1));
	}

	void AddCallback(void(*const function)(std::string inputLine))
	{
		m_callbacks.emplace_back(function);
	}

	using InputCallback = std::function<void(std::string)>;
	std::vector<InputCallback> m_callbacks;
protected:
private:
	bool m_isRunning = false;
	std::thread* m_listenerThread;
};
