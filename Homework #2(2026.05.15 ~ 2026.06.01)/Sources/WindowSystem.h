#pragma once

#include <expected>
#include <memory>
#include <vector>

#include "SystemBase.h"
#include "Window.h"

class EventQueue;

class WindowSystem : public SystemBase
{
public:
	~WindowSystem() override = default;

	std::expected<void, std::wstring> Initialize(const EngineContext& context_) override;
	void Release() override;

	void PollEvents(EventQueue& eventQueue_);

	auto GetWindows(this const auto& self_) noexcept;
	auto GetActiveWindows(this const auto& self_) noexcept;

private:
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);

	std::vector<std::unique_ptr<Window>> windows;
};

inline auto WindowSystem::GetWindows(this const auto& self_) noexcept
{
	return self_.windows
		| std::views::transform([](const std::unique_ptr<Window>& window_) noexcept -> const Window*
			{
				return window_.get();
			});
}

inline auto WindowSystem::GetActiveWindows(this const auto& self_) noexcept
{
	return self_.windows
		| std::views::filter([](const std::unique_ptr<Window>& window_) noexcept
			{
				return window_ != nullptr;
			})
		| std::views::transform([](const std::unique_ptr<Window>& window_) noexcept -> const Window*
			{
				return window_.get();
			});
}
