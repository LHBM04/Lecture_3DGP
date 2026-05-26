#pragma once

#include "System.h"
#include "Window.h"

#ifdef CreateWindow
#undef CreateWindow
#endif

class WindowSystem final : public System<WindowOptions>
{
public:
	~WindowSystem() noexcept override;

	bool Initialize(const WindowOptions& options_) override;
	void Release() override;

	[[nodiscard]] Window* CreateWindow(const WindowOptions& options_);
	void DestroyWindow(Window*& window_) noexcept;

	[[nodiscard]] Window& GetMainWindow();
	[[nodiscard]] const Window& GetMainWindow() const;

private:
	std::unique_ptr<Window> mainWindow;
};
