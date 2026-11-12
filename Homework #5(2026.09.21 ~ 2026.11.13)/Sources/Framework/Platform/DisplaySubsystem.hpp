#pragma once

#include "../Core/Common.hpp"
#include "../Core/EngineSubsystem.hpp"

#include "Display.hpp"
#include "DisplayOptions.hpp"

namespace TUK::Framework
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	class DisplaySubsystem : public EngineSubsystem
	{
	public:
		DisplaySubsystem() noexcept;

		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

		[[nodiscard]] std::expected<Display*, std::string> CreateDisplay(const DisplayOptions& options) noexcept;
		void DestroyDisplay(Display* display) noexcept;

		[[nodiscard]] auto GetDisplays() noexcept;
		[[nodiscard]] auto GetDisplays() const noexcept;

		[[nodiscard]] usize GetDisplayCount() const noexcept;

		[[nodiscard]] Display* GetDisplay(usize index) noexcept;
		[[nodiscard]] const Display* GetDisplay(usize index) const noexcept;

		[[nodiscard]] Display* FindDisplayByHWND(HWND hWnd) noexcept;
		[[nodiscard]] const Display* FindDisplayByHWND(HWND hWnd) const noexcept;

		[[nodiscard]] Display* FindDisplayByID(Display::ID id) noexcept;
		[[nodiscard]] const Display* FindDisplayByID(Display::ID id) const noexcept;

		[[nodiscard]] bool IsDisplayValid(const Display* display) const noexcept;
		[[nodiscard]] bool IsDisplayValid(HWND hWnd) const noexcept;
		[[nodiscard]] bool IsDisplayValid(Display::ID id) const noexcept;

	private:
		static constexpr usize WNDCLASS_NAME_LENGTH = 256;

		HINSTANCE instance;

		TCHAR wndClassName[WNDCLASS_NAME_LENGTH];
		ATOM wndClass;

		std::vector<std::unique_ptr<Display>> displays;
	};

	inline DisplaySubsystem::DisplaySubsystem() noexcept
		: EngineSubsystem(10)
		, instance(GetModuleHandle(nullptr))
		, wndClassName{}
		, wndClass{}
	{
	}

	inline auto DisplaySubsystem::GetDisplays() noexcept
	{
		return displays | std::views::transform([](const std::unique_ptr<Display>& displayPtr) -> Display*
			{
				return displayPtr.get();
			});
	}

	inline auto DisplaySubsystem::GetDisplays() const noexcept
	{
		return displays | std::views::transform([](const std::unique_ptr<Display>& displayPtr) -> const Display*
			{
				return displayPtr.get();
			});
	}
}
