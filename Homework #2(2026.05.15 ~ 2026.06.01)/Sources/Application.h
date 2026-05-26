#pragma once

#include "InputSystem.h"
#include "ResourceSystem.h"
#include "RenderSystem.h"
#include "SceneSystem.h"
#include "WindowSystem.h"

#include <concepts>
#include <typeindex>
#include <unordered_map>
#include <vector>

struct ApplicationOptions final
{
	std::wstring title;
	int width;
	int height;
	bool fullscreen;
	bool borderless;
	bool resizable;
};

class Application final
{
public:
	Application() noexcept;
	~Application() noexcept;

	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	Application(Application&&) = delete;
	Application& operator=(Application&&) = delete;

	bool Initialize(const ApplicationOptions& options_);
	int Run();
	void Quit() noexcept;

private:
	void ShutdownSystems() noexcept;
	void DispatchKeyboardInput() noexcept;
	void ReleaseSystemsInReverseOrder() noexcept;

	template<std::derived_from<System> TSystem>
	TSystem& AddSystem();

	template<std::derived_from<System> TSystem>
	TSystem& GetSystemRef() noexcept;

	template<std::derived_from<System> TSystem>
	const TSystem& GetSystemRef() const noexcept;

private:
	bool isRunning;
	bool isInitialized;
	float fixedUpdateAccumulator;

	std::unordered_map<std::type_index, std::unique_ptr<System>> systems;
	std::vector<std::type_index> systemOrder;

	RenderTargetHandle mainRenderTarget;
};

template<std::derived_from<System> TSystem>
inline TSystem& Application::AddSystem()
{
	const std::type_index type{ typeid(TSystem) };
	auto found{ systems.find(type) };
	if (found != systems.end())
	{
		return *static_cast<TSystem*>(found->second.get());
	}

	std::unique_ptr<TSystem> created{ std::make_unique<TSystem>() };
	TSystem* raw{ created.get() };

	systems.emplace(type, std::move(created));
	systemOrder.push_back(type);
	return *raw;
}

template<std::derived_from<System> TSystem>
inline TSystem& Application::GetSystemRef() noexcept
{
	const std::type_index type{ typeid(TSystem) };
	auto found{ systems.find(type) };
	assert(found != systems.end());
	return *static_cast<TSystem*>(found->second.get());
}

template<std::derived_from<System> TSystem>
inline const TSystem& Application::GetSystemRef() const noexcept
{
	const std::type_index type{ typeid(TSystem) };
	auto found{ systems.find(type) };
	assert(found != systems.end());
	return *static_cast<const TSystem*>(found->second.get());
}
