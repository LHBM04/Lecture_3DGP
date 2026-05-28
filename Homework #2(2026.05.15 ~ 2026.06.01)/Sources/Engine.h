#pragma once

#include "EngineOptions.h"
#include "EventQueue.h"
#include "System.h"

class Event;
class InputSystem;
class RenderContext;
class Renderer;
class RenderSystem;
class SceneSystem;
class TimeSystem;
class WindowSystem;
class WindowCloseEvent;
class WindowMaximizeEvent;
class WindowMinimizeEvent;
class WindowMoveEvent;
class WindowResizeEvent;

class Engine
{
public:
	Engine() = default;
	~Engine() = default;

	Engine(const Engine&) = delete;
	Engine operator=(const Engine&) = delete;

	Engine(Engine&&) = delete;
	Engine operator=(Engine&&) = delete;

	bool Initialize(const EngineOptions& options_);
	void Release();
	
	int Run();

	template <std::derived_from<ISystem> TSystem>
	TSystem* AddSystem();

	template <std::derived_from<ISystem> TSystem>
	TSystem* GetSystem() const noexcept;

private:
	bool OnEvent(Event& event);
	bool OnWindowClose(WindowCloseEvent& event);
	bool OnWindowMaximize(WindowMaximizeEvent& event);
	bool OnWindowMinimize(WindowMinimizeEvent& event);
	bool OnWindowMove(WindowMoveEvent& event);
	bool OnWindowResize(WindowResizeEvent& event);

private:
	std::unordered_map<std::type_index, std::unique_ptr<ISystem>> systems;

	bool isRunning{ false };
	EventQueue eventQueue;

	WindowSystem* windowSystem{ nullptr };
	RenderSystem* renderSystem{ nullptr };
	Renderer* renderer{ nullptr };
	RenderContext* renderContext{ nullptr };
	TimeSystem* timeSystem{ nullptr };
	InputSystem* inputSystem{ nullptr };
	SceneSystem* sceneSystem{ nullptr };
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
