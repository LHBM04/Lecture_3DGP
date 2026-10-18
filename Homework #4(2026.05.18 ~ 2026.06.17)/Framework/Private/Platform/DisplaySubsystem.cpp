#include "Precompiled.hpp"
#include "DisplaySubsystem.hpp"

#include "Display_Impl.hpp"
#include "WindowProc.hpp"

namespace
{
	/** 윈도우 클래스 이름 */
	constexpr LPCTSTR ClassName = TEXT("TUK.Framework.Window");

	/** 인스턴스 핸들 */
	const HINSTANCE Instance = GetModuleHandleW(nullptr);

	/** 윈도우 클래스 */
	bool isWndClassRegistered = false;
}

namespace TUK::Framework
{
	DisplaySubsystem::DisplaySubsystem() noexcept
		: EngineSubsystem(2)
	{
	}

	DisplaySubsystem::~DisplaySubsystem() noexcept
	{
		displays.clear();

		if (isWndClassRegistered)
		{
			UnregisterClassW(ClassName, Instance);
			isWndClassRegistered = false;
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

	SystemStatus DisplaySubsystem::OnStartup()
	{
		WNDCLASSEXW description{};
		description.cbSize = sizeof(description);
		description.style = CS_HREDRAW | CS_VREDRAW;
		description.lpfnWndProc = WindowProc;
		description.hInstance = Instance;
		description.hCursor = LoadCursor(nullptr, IDC_ARROW);
		description.lpszClassName = ClassName;
		description.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		
		if (RegisterClassEx(&description) == 0)
		{
			return SystemStatus::Failure;
		}

		isWndClassRegistered = true;

		return SystemStatus::Continue;
	}

	SystemStatus DisplaySubsystem::OnPostTick()
	{
		std::erase_if(displays, [](const auto& display) {
			return display->ShouldClose();
		});

		return !displays.empty() ? SystemStatus::Continue : SystemStatus::Success;
	}

	void DisplaySubsystem::OnShutdown()
	{
		if (isWndClassRegistered)
		{
			UnregisterClassW(ClassName, Instance);
			isWndClassRegistered = false;
		}
	}
}
