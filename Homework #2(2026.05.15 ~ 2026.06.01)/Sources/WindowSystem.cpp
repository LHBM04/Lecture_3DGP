#include "Precompiled.h"
#include "WindowSystem.h"

WindowSystem::~WindowSystem() noexcept
{
	Release();
}

bool WindowSystem::Initialize(const WindowOptions& options_)
{
	return nullptr != CreateWindow(options_);
}

void WindowSystem::Release()
{
	if (mainWindow)
	{
		mainWindow->Release();
		mainWindow.reset();
	}
}

Window* WindowSystem::CreateWindow(const WindowOptions& options_)
{
	if (mainWindow)
	{
		return mainWindow.get();
	}

	std::unique_ptr<Window> window{ std::make_unique<Window>() };
	if (!window->Initialize(options_))
	{
		return nullptr;
	}

	mainWindow = std::move(window);
	return mainWindow.get();
}

void WindowSystem::DestroyWindow(Window*& window_) noexcept
{
	if (nullptr == window_ || !mainWindow || window_ != mainWindow.get())
	{
		return;
	}

	mainWindow->Release();
	mainWindow.reset();
	window_ = nullptr;
}

Window& WindowSystem::GetMainWindow()
{
	assert(mainWindow);
	return *mainWindow;
}

const Window& WindowSystem::GetMainWindow() const
{
	assert(mainWindow);
	return *mainWindow;
}
