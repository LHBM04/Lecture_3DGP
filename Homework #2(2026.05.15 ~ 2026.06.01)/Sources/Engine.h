#pragma once

#include "System.h"

class InputSystem;
class RenderSystem;
class TimeSystem;
class WindowSystem;

class Engine
{
public:
	struct Options final
	{
		std::wstring title;
		int x;
		int y;
		int width;
		int height;
		bool fullscreen;
		bool resizable;
		bool borderless;
		float fixedTime;
	};

	Engine() = default;
	~Engine() = default;

	Engine(const Engine&) = delete;
	Engine operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine operator=(Engine&&) = delete;

	bool Initialize(const Options& options_);
	void Release();
	
	int Run();

	template <std::derived_from<ISystem> TSystem>
	TSystem* AddSystem();

	template <std::derived_from<ISystem> TSystem>
	TSystem* GetSystem() const noexcept;

private:
	std::unordered_map<std::type_index, std::unique_ptr<ISystem>> systems;

	WindowSystem* windowSystem;
	RenderSystem* renderSystem;
	TimeSystem* timeSystem;
	InputSystem* inputSystem;
};

template <std::derived_from<ISystem> TSystem>
inline TSystem* Engine::AddSystem()
{
	std::type_index index{ typeid(TSystem) };

	if (const auto it = systems.find(index); it != systems.end())
	{
		return static_cast<TSystem*>(it->second.get());
	}

	auto [it, inserted] = systems.emplace(index, std::make_unique<TSystem>());
	return static_cast<TSystem*>(it->second.get());
}

template <std::derived_from<ISystem> TSystem>
inline TSystem* Engine::GetSystem() const noexcept
{
	std::type_index index{ typeid(TSystem) };
	if (systems.contains(index))
	{
		return static_cast<TSystem*>(systems[index].get());
	}

	return nullptr;
}
