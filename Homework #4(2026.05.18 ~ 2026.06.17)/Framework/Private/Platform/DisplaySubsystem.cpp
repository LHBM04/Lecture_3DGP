#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

#include "../Core/Engine.hpp"
#include "Display_Impl.hpp"

namespace
{
	/** 윈도우 클래스 이름 */
	constexpr LPCTSTR ClassName = TEXT("TUK.Framework.Window");

	/** 인스턴스 핸들 */
	const HINSTANCE Instance = GetModuleHandleW(nullptr);

	/** 윈도우 클래스 */
	ATOM wndClassEx;
}

namespace TUK::Framework
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* window = reinterpret_cast<Display*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (message == WM_NCCREATE)
		{
			const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(lParam);

			window = static_cast<Display*>(creation->lpCreateParams);
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

	DisplaySubsystem::DisplaySubsystem() noexcept
		: EngineSubsystem(2)
	{
	}

	DisplaySubsystem::~DisplaySubsystem() noexcept
	{
		displays.clear();

		if (wndClassEx == 0)
		{
			UnregisterClassW(ClassName, Instance);
			wndClassEx = 0;
		}
	}

	std::expected<std::reference_wrapper<Display>, std::string> DisplaySubsystem::CreateDisplay(const DisplayOptions& options)
	{
		std::unique_ptr<Display> newWindow = std::unique_ptr<Display>(new Display_Impl(nullptr));

		HWND hWnd = CreateWindowEx(
			0, 
			ClassName, 
			options.caption.data(),
			WS_OVERLAPPEDWINDOW,
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
		displays.push_back(std::move(newWindow));
		ShowWindow(hWnd, SW_SHOW);
		return reference;
	}

	const std::vector<std::unique_ptr<Display>>& DisplaySubsystem::GetDisplays() const noexcept
	{
		return displays;
	}

	void DisplaySubsystem::OnStartup()
	{
		WNDCLASSEXW description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = WindowProc;
		description.hInstance = Instance;
		description.hCursor = LoadCursor(nullptr, IDC_ARROW);
		description.lpszClassName = ClassName;
		description.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wndClassEx = RegisterClassExW(&description);
	}

	void DisplaySubsystem::PreTick()
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

	void DisplaySubsystem::PostTick()
	{
		std::erase_if(displays, [](const auto& display)
		{
			return display->ShouldClose();
		});

		if (displays.empty() && Engine::GetInstance()->IsRunning())
		{
			Engine::GetInstance()->RequestQuit(EXIT_SUCCESS);
		}
	}

	void DisplaySubsystem::OnShutdown()
	{
		displays.clear();

		if (wndClassEx)
		{
			UnregisterClassW(ClassName, Instance);
			wndClassEx = 0;
		}
	}
}
