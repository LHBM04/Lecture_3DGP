#include "Precompiled.hpp"
#include "WindowSubsystem.hpp"

#include "../Core/Engine.hpp"
#include "Window_Impl.hpp"

namespace
{
	/** 윈도우 클래스 이름 */
	constexpr LPCTSTR ClassName = TEXT("TUK.Framework.Window");

	/** 인스턴스 핸들 */
	const HINSTANCE Instance = GetModuleHandleW(nullptr);

	/** 윈도우 클래스 */
	ATOM windowClass;
}

namespace TUK::Framework
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);

			window = static_cast<Window*>(creation->lpCreateParams);
			if (!window)
			{
				return FALSE;
			}

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		}
		else if (message == WM_NCDESTROY)
		{
			SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		}

		if (window)
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	WindowSubsystem::WindowSubsystem() noexcept
		: EngineSubsystem(2)
	{
	}

	WindowSubsystem::~WindowSubsystem() noexcept
	{
		windows.clear();

		if (windowClass == 0)
		{
			UnregisterClassW(ClassName, Instance);
			windowClass = 0;
		}
	}

	std::expected<std::reference_wrapper<Window>, std::string> WindowSubsystem::Create(const DisplayOptions& options)
	{
		std::unique_ptr<Window> newWindow = std::unique_ptr<Window>(new Window_Impl(nullptr));

		HWND hWnd = CreateWindowExW(
			0, 
			ClassName, 
			options.caption,
			options.style,
			options.position.GetX(),
			options.position.GetY(),
			options.size.GetX(),
			options.size.GetY(),
			nullptr, 
			nullptr, 
			Instance, 
			newWindow.get());

		if (!hWnd)
		{
			return std::unexpected(std::format("창 생성 실패 (Win32 오류: {}).", GetLastError()));
		}

		auto reference = std::ref(*newWindow);
		windows.push_back(std::move(newWindow));
		ShowWindow(hWnd, SW_SHOW);
		return reference;
	}

	const std::vector<std::unique_ptr<Window>>& WindowSubsystem::GetWindows() const noexcept
	{
		return windows;
	}

	void WindowSubsystem::OnStartup()
	{
		WNDCLASSEXW description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = WindowProc;
		description.hInstance = Instance;
		description.hCursor = LoadCursor(nullptr, IDC_ARROW);
		description.lpszClassName = ClassName;
		description.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		windowClass = RegisterClassExW(&description);
	}

	void WindowSubsystem::PreTick()
	{
		MSG message{};
		while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				Engine::GetInstance()->RequestQuit(static_cast<int>(message.wParam));
				break;
			}

			TranslateMessage(&message);
			DispatchMessageW(&message);
		}
	}

	void WindowSubsystem::PostTick()
	{
		std::erase_if(windows, [](const auto& window)
		{
			return window->ShouldClose();
		});

		if (windows.empty() && !hasExitRequest && Engine::GetInstance()->IsRunning())
		{
			hasExitRequest = true;
			Engine::GetInstance()->RequestQuit(EXIT_SUCCESS);
		}
	}

	void WindowSubsystem::OnShutdown()
	{
		windows.clear();

		if (windowClass)
		{
			UnregisterClassW(ClassName, Instance);
			windowClass = 0;
		}
	}
}
