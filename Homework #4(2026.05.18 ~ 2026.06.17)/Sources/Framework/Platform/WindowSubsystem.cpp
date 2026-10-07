#include "Precompiled.h"
#include "WindowSubsystem.h"

#include "../Core/Engine.h"
#include "EventSubsystem.h"

namespace
{
	constexpr wchar_t ClassName[] = L"TUK.Framework.Window";
	const HINSTANCE instance = GetModuleHandleW(nullptr);

}

namespace TUK::Framework
{
	WindowSubsystem::WindowSubsystem() noexcept
		: EngineSubsystem(2)
		, windowClass(0)
		, hasExitRequest(false)
		, windows()
	{
	}

	WindowSubsystem::~WindowSubsystem() noexcept
	{
		OnShutdown();
	}

	std::expected<std::reference_wrapper<Window>, std::string> WindowSubsystem::Create(const WindowOptions& options)
	{
		if (!windowClass || hasExitRequest || !Engine::GetInstance().IsRunning())
		{
			return std::unexpected(std::string{ "창을 생성할 수 있는 상태가 아닙니다." });
		}
		if (options.size.GetX() <= 0 || options.size.GetY() <= 0)
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
			options.position.GetX(), 
			options.position.GetY(), 
			options.size.GetX(), 
			options.size.GetY(),
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
			hasExitRequest = true;
			Engine::GetInstance().ReportError(std::format("창 클래스 등록 실패 (Win32 오류: {}).", GetLastError()));
			return;
		}

		auto& system = Engine::GetInstance();
		WindowOptions options;
		const auto& title = system.GetOption<std::string>("Window.Title");
		if (title.size() > static_cast<std::size_t>((std::numeric_limits<int>::max)()))
		{
			hasExitRequest = true;
			system.ReportError("Window.Title이 너무 깁니다.");
			return;
		}
		if (title.empty())
		{
			options.title.clear();
		}
		else
		{
			const int length = static_cast<int>(title.size());
			const int wideLength = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
				title.data(), length, nullptr, 0);
			if (wideLength == 0)
			{
				hasExitRequest = true;
				system.ReportError("Window.Title은 올바른 UTF-8 문자열이어야 합니다.");
				return;
			}
			options.title.resize(wideLength);
			if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
				title.data(), length, options.title.data(), wideLength) == 0)
			{
				hasExitRequest = true;
				system.ReportError("Window.Title의 UTF-16 변환에 실패했습니다.");
				return;
			}
		}

		options.position.Set(system.GetOption<int>("Window.PositionX"), system.GetOption<int>("Window.PositionY"));
		options.size.Set(system.GetOption<int>("Window.SizeX"), system.GetOption<int>("Window.SizeY"));
		options.isResizable = system.GetOption<bool>("Window.IsResizable");
		options.hasMinimizeButton = system.GetOption<bool>("Window.HasMinimizeButton");
		options.hasMaximizeButton = system.GetOption<bool>("Window.HasMaximizeButton");
		options.isAlwaysOnTop = system.GetOption<bool>("Window.IsAlwaysOnTop");
		options.isVisible = system.GetOption<bool>("Window.IsVisible");
		const auto window = Create(options);
		if (!window)
		{
			hasExitRequest = true;
			system.ReportError(window.error());
		}
	}

	void WindowSubsystem::OnPostTick()
	{
		std::erase_if(windows, [](const auto& window)
		{
			return window->ShouldClose();
		});

		if (windows.empty() && !hasExitRequest && Engine::GetInstance().IsRunning())
		{
			hasExitRequest = true;
			Engine::GetInstance().RequestQuit(EXIT_SUCCESS);
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
