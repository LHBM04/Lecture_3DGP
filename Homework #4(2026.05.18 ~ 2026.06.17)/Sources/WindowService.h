#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef IsMinimized
#undef IsMaximized

#include "Service.h"

class WindowService : public Service
{
public:
	struct Options final
	{
		std::string title;
		int x;
		int y;
		int width;
		int height;
		bool isFullscreen;
		bool isResizable;
		bool isBorderless;
	};

	~WindowService() noexcept override = default;

	bool Initialize(const Options& options_);
	void Terminate() noexcept;

	void Show() noexcept;
	void Hide() noexcept;

	[[nodiscard]] HWND GetHWND() const noexcept;
	[[nodiscard]] HINSTANCE GetHINSTANCE() const noexcept;

	[[nodiscard]] const std::string& GetTitle() const noexcept;
	void SetTitle(std::string_view title_) noexcept;

	[[nodiscard]] int GetWidth() const noexcept;
	void SetWidth(int width_) noexcept;

	[[nodiscard]] int GetHeight() const noexcept;
	void SetHeight(int height_) noexcept;

	[[nodiscard]] SIZE GetSize() const noexcept;
	void SetSize(SIZE size_) noexcept;

	[[nodiscard]] int GetX() const noexcept;
	void SetX(int x_) noexcept;

	[[nodiscard]] int GetY() const noexcept;
	void SetY(int y_) noexcept;

	[[nodiscard]] POINT GetPosition() const noexcept;
	void SetPosition(POINT position_) noexcept;

	[[nodiscard]] bool IsFullscreen() const noexcept;
	void SetFullscreen(bool fullscreen_) noexcept;

	[[nodiscard]] bool IsResizable() const noexcept;
	void SetResizable(bool resizable_) noexcept;

	[[nodiscard]] bool IsBorderless() const noexcept;
	void SetBorderless(bool decorated_) noexcept;

	[[nodiscard]] bool IsMinimized() const noexcept;
	void Minimize() noexcept;

	[[nodiscard]] bool IsMaximized() const noexcept;
	void Maximize() noexcept;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	Options options;

	HINSTANCE hInstance;
	HWND hWnd;
};
