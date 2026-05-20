#pragma once

class Window final
{
	STATIC_CLASS(Window)
public:
	struct Options final
	{
		HINSTANCE hInstance;
		LPCWSTR className;
		LPCWSTR title;
		float x;
		float y;
		float width;
		float height;
		bool isFullscreen;
		bool isResizable;
		bool isBorderless;
		bool isVSync;
	};

	[[nodiscard]] static HWND GetHWND();

	[[nodiscard]] static bool ShouldClose();
	static void SetShouldClose(bool shouldClose_);

	[[nodiscard]] static LPCWSTR GetTitle();
	static void SetTitle(LPCWSTR title_);

	[[nodiscard]] static DWORD GetWindowStyle();
	[[nodiscard]] static DWORD GetWindowExStyle();	

	static bool Initialize(const Options& options_);
	static void PollEvent();
	static void Terminate();

private:
	static LRESULT CALLBACK WndProc(HWND hWnd_, UINT uMsg_, WPARAM wParam_, LPARAM lParam_);

	static Options options;
	static HWND hWnd;
	static bool shouldClose;
};