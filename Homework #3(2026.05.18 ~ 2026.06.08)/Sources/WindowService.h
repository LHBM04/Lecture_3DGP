#pragma once

#include <unordered_set>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "Service.h"
#include "WindowOptions.h"

class WindowService : public Service
{
public:
	~WindowService() override = default;

	HWND AddWindow(const WindowOptions& options_);
	void RemoveWindow(HWND window_);

	bool PollEvents();

	[[nodiscard]] HWND GetMainWindow() const noexcept;
	[[nodiscard]] const std::unordered_set<HWND>& GetWindows() const noexcept;
	void SetMainWindow(HWND window_) noexcept;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	std::unordered_set<HWND> windows;
	HWND mainWindow{ nullptr };
};
