#include "Precompiled.hpp"
#include "WindowSubsystem.hpp"

#include "../Core/Engine.hpp"
#include "EventSubsystem.hpp"

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
		assert(options.size.GetX() > 0 && options.size.GetY() > 0);
		if (!windowClass || hasExitRequest)
		{
			return std::unexpected(std::string("창 서브시스템이 초기화되지 않았거나 종료 중입니다."));
		}

		WindowOptions resolvedOptions = options;
		DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
		if (options.isBorderless || options.isFullscreen)
		{
			style = WS_POPUP;
		}
		if (!options.isFullscreen)
		{
			style |= WS_SYSMENU | WS_MINIMIZEBOX;
			if (options.isResizable)
			{
				style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
			}
		}
		else
		{
			const POINT position{
				options.position.GetX() == CW_USEDEFAULT ? 0 : options.position.GetX(),
				options.position.GetY() == CW_USEDEFAULT ? 0 : options.position.GetY()
			};
			MONITORINFO monitor{};
			monitor.cbSize = sizeof(monitor);
			if (!GetMonitorInfoW(MonitorFromPoint(position, MONITOR_DEFAULTTONEAREST), &monitor))
			{
				return std::unexpected(std::format("모니터 조회 실패 (Win32 오류: {}).", GetLastError()));
			}
			resolvedOptions.position.Set(monitor.rcMonitor.left, monitor.rcMonitor.top);
			if (options.isBorderless)
			{
				resolvedOptions.size.Set(monitor.rcMonitor.right - monitor.rcMonitor.left,
					monitor.rcMonitor.bottom - monitor.rcMonitor.top);
			}
		}

		std::unique_ptr<Window> window = std::make_unique<Window>(resolvedOptions);
		const HWND hWnd = CreateWindowExW(0, ClassName, resolvedOptions.title.c_str(), style,
			resolvedOptions.position.GetX(), resolvedOptions.position.GetY(),
			resolvedOptions.size.GetX(), resolvedOptions.size.GetY(),
			nullptr, nullptr, instance, window.get());
		if (!hWnd)
		{
			return std::unexpected(std::format("창 생성 실패 (Win32 오류: {}).", GetLastError()));
		}

		auto reference = std::ref(*window);
		windows.push_back(std::move(window));
		ShowWindow(hWnd, SW_SHOW);
		return reference;
	}

	const std::vector<std::unique_ptr<Window>>& WindowSubsystem::GetWindows() const noexcept
	{
		return windows;
	}

	void WindowSubsystem::OnStartup()
	{
		hasExitRequest = false;
		auto& engine = Engine::GetInstance();
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
			engine.ReportError(std::format("창 클래스 등록 실패 (Win32 오류: {}).", GetLastError()));
			return;
		}

		WindowOptions options;
		const auto& title = engine.GetOption<std::string>("Window.Title");
		assert(title.size() <= static_cast<std::size_t>(std::numeric_limits<int>::max()));
		if (!title.empty())
		{
			const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
				title.data(), static_cast<int>(title.size()), nullptr, 0);
			if (length == 0)
			{
				hasExitRequest = true;
				engine.ReportError(std::format("창 제목 UTF-8 변환 실패 (Win32 오류: {}).", GetLastError()));
				return;
			}
			options.title.resize(length);
			if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, title.data(),
				static_cast<int>(title.size()), options.title.data(), length) == 0)
			{
				hasExitRequest = true;
				engine.ReportError(std::format("창 제목 UTF-8 변환 실패 (Win32 오류: {}).", GetLastError()));
				return;
			}
		}
		else
		{
			options.title.clear();
		}
		options.position.Set(engine.GetOption<int>("Window.PositionX"), engine.GetOption<int>("Window.PositionY"));
		options.size.Set(engine.GetOption<int>("Window.SizeX"), engine.GetOption<int>("Window.SizeY"));
		options.isResizable = engine.GetOption<bool>("Window.IsResizable");
		options.isBorderless = engine.GetOption<bool>("Window.IsBorderless");
		options.isFullscreen = engine.GetOption<bool>("Window.IsFullscreen");

		const auto window = Create(options);
		if (!window)
		{
			hasExitRequest = true;
			engine.ReportError(window.error());
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
		windows.clear();
		if (windowClass)
		{
			UnregisterClassW(ClassName, instance);
			windowClass = 0;
		}
	}
}
