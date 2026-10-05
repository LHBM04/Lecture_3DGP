#include "Precompiled.h"
#include "WindowSubsystem.h"
#include "EventSubsystem.h"

#include "../Core/System.h"

namespace
{
	constexpr wchar_t ClassName[] = L"TUK.Framework.Window";
	const HINSTANCE instance = GetModuleHandleW(nullptr);

}

namespace TUK::Framework
{
	WindowSubsystem::WindowSubsystem() noexcept
		: Subsystem(2)
		, windowClass(0)
		, hasExitRequest(false)
		, windows()
	{
	}

	WindowSubsystem::~WindowSubsystem() noexcept
	{
		windows.clear();
		if (windowClass)
		{
			UnregisterClassW(ClassName, instance);
			windowClass = 0;
		}
	}

	std::expected<std::reference_wrapper<Window>, std::string> WindowSubsystem::Create(const WindowOptions& options)
	{
		if (!windowClass || hasExitRequest || !System::GetInstance().IsRunning())
		{
			return std::unexpected(std::string{ "창을 생성할 수 있는 상태가 아닙니다." });
		}
		if (options.sizeX <= 0 || options.sizeY <= 0)
		{
			return std::unexpected(std::string{ "창의 크기는 0보다 커야 합니다." });
		}

		DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
		if (options.isResizable)
		{
			style |= WS_THICKFRAME;
		}
		if (options.hasMinimizeButton)
		{
			style |= WS_MINIMIZEBOX;
		}
		if (options.hasMaximizeButton)
		{
			style |= WS_MAXIMIZEBOX;
		}
		const DWORD extendedStyle = options.isAlwaysOnTop ? WS_EX_TOPMOST : 0;

		auto window = std::make_unique<Window>(options);

		const HWND hWnd = CreateWindowExW(
			extendedStyle, 
			ClassName, 
			options.title.data(), 
			style,
			options.positionX, 
			options.positionY, 
			options.sizeX, 
			options.sizeY,
			nullptr, 
			nullptr, 
			instance, 
			window.get());

		if (!hWnd)
		{
			const DWORD error = GetLastError();
			return std::unexpected(std::format("창 생성 실패 (Win32 오류: {}).", error));
		}

		auto reference = std::ref(*window);
		windows.push_back(std::move(window));
		if (options.isVisible)
		{
			ShowWindow(hWnd, SW_SHOW);
		}
		return reference;
	}

	const std::vector<std::unique_ptr<Window>>& WindowSubsystem::GetWindows() const noexcept
	{
		return windows;
	}

	void WindowSubsystem::OnStartup()
	{
		if (windowClass)
		{
			return;
		}

		hasExitRequest = false;

		WNDCLASSEXW description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = EventSubsystem::GetWindowProc();
		description.hInstance = instance;
		description.hCursor = LoadCursorW(nullptr, IDC_ARROW);
		description.lpszClassName = ClassName;
		windowClass = RegisterClassExW(&description);
		if (!windowClass)
		{
			OutputDebugStringW(L"WindowSubsystem: 창 클래스 등록 실패.\n");
			hasExitRequest = true;
			System::GetInstance().RequestQuit(EXIT_FAILURE);
			return;
		}

		auto& system = System::GetInstance();
		const auto options = system.GetOption<WindowOptions>("Window.Options");
		if (!options)
		{
			OutputDebugStringA(options.error().c_str());
			hasExitRequest = true;
			system.RequestQuit(EXIT_FAILURE);
			return;
		}

		const auto window = Create(options->get());
		if (!window)
		{
			OutputDebugStringA(window.error().c_str());
			hasExitRequest = true;
			system.RequestQuit(EXIT_FAILURE);
		}
	}

	void WindowSubsystem::OnPostTick()
	{
		std::erase_if(windows, [](const auto& window)
		{
			return window->ShouldClose();
		});

		if (windows.empty() && !hasExitRequest && System::GetInstance().IsRunning())
		{
			hasExitRequest = true;
			System::GetInstance().RequestQuit(EXIT_SUCCESS);
		}
	}

	void WindowSubsystem::OnShutdown()
	{
		// 해당 클래스의 창을 모두 파괴한 뒤 클래스를 해제한다.
		windows.clear();
		if (windowClass)
		{
			UnregisterClassW(ClassName, instance);
			windowClass = 0;
		}
	}
}
