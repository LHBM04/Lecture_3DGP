#pragma once

#include "Renderer.h"

class Engine final
{
public:
	struct Options final
	{
		LPCWSTR title{ L"New Application" };
		int width{ 800 };
		int height{ 600 };
		bool resizable{ true };
		bool fullscreen{ false };
		bool borderless{ false };
	};

	Engine() = default;
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	~Engine() = default;

	bool Initialize(HINSTANCE instance_) noexcept;
	int Run(const Options& options_);

	[[nodiscard]] bool ShouldClose() const noexcept;
	void SetShouldClose(bool shouldClose_) noexcept;

	[[nodiscard]] LPCWSTR GetTitle() const noexcept;
	void SetTitle(LPCWSTR title_) noexcept;

	[[nodiscard]] int GetWidth() const noexcept;
	void SetWidth(int width_) noexcept;

	[[nodiscard]] int GetHeight() const noexcept;
	void SetHeight(int height_) noexcept;

	[[nodiscard]] bool IsResizable() const noexcept;
	void SetResizable(bool resizable_) noexcept;

	[[nodiscard]] bool IsFullscreen() const noexcept;
	void SetFullscreen(bool fullscreen_) noexcept;

	[[nodiscard]] bool IsBorderless() const noexcept;
	void SetBorderless(bool borderless_) noexcept;

	void Resize(int width_, int height_) noexcept;

private:
	static constexpr LPCWSTR WINDOW_CLASS_NAME{ L"Homework #3 Class" };

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

	[[nodiscard]] DWORD ResolveWindowStyle() const noexcept;
	void ApplyWindowMode() noexcept;

	Options options_{};
	HINSTANCE instance_{ nullptr };
	HWND window_{ nullptr };
	bool shouldClose_{ true };
};
