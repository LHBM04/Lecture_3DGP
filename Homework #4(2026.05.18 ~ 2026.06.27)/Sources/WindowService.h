#pragma once

#include <memory>
#include <vector>

#include "Service.h"
#include "Window.h"

class WindowService : public Service
{
public:
	WindowService() noexcept = default;
	~WindowService() noexcept override = default;

	Window* AddWindow(const WindowOptions& options_);
	void RemoveWindow(Window& window_);

	Window* GetMainWindow() const noexcept;
	void SetMainWindow(Window& window_) noexcept;

	bool PollEvents();

protected:
	virtual void OnAdd() override;
	virtual void OnRemove() override;

private:
	std::vector<std::unique_ptr<Window>> windows;
	Window* mainWindow{ nullptr };
};
