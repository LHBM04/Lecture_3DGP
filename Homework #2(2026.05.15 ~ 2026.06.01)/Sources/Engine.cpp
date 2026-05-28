#include "Precompiled.h"
#include "Engine.h"

std::expected<void, std::wstring> Engine::Initialize(const EngineOptions& options_)
{
	return {};
}

void Engine::Release()
{

}

int Engine::Run()
{
	EventBase event;
	while (!windowSystem.PeekEvent(event))
	{
		windowSystem.TranslateEvent(evnet);
		eventSystem.DispatchEvent(event);
	}

	return 0;
}
