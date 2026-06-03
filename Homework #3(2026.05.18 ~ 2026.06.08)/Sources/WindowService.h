#pragma once

#include <unordered_set>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "Service.h"

struct WindowOptions final
{
	std::wstring title;
	int x;
	int y;
	int width;
	int height;
	bool isFullscreen;
	bool isBorderless;
	bool isResizable;
};

class WindowService : public Service
{
public:
	~WindowService() override = default;

	HWND AddWindow(const WindowOptions& options_);
	void RemoveWindow(HWND window_);

	bool PollEvents();

	[[nodiscard]] HWND GetMainWindow() const noexcept;
	void SetMainWindow(HWND window_) noexcept;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	std::unordered_set<HWND> windows;
	HWND mainWindow{ nullptr };
};