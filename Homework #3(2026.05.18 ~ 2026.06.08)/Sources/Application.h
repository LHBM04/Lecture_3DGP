#pragma once

#include "Renderer.h"

class Application final
{
public:
	struct Options final
	{
		HINSTANCE instance{ nullptr };
		INT showCommand{ SW_SHOW };
		UINT width{ 1280 };
		UINT height{ 720 };
		LPCWSTR title{ L"Homework #3 - DX12" };
	};

	Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	static bool Initialize(HINSTANCE instance, const Options& options = {});
	static int Run();
	static void Shutdown() noexcept;

private:
	static LRESULT CALLBACK WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

	bool InitializeInstance(const Options& options);
	int RunInstance();
	void ShutdownInstance() noexcept;

	bool CreateMainWindow(const Options& options);

	Renderer renderer;
	HINSTANCE instance{ nullptr };
	HWND windowHandle{ nullptr };
	UINT width{ 0 };
	UINT height{ 0 };
	bool running{ false };
};
