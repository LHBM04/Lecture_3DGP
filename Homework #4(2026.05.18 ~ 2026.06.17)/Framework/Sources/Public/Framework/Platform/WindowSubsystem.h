#pragma once

#include <expected>
#include <memory>
#include <system_error>
#include <type_traits>
#include <vector>

#include "../Core/Subsystem.h"

namespace TUK::Framework
{
	struct WindowOptions;
	class Window;

	class WindowSubsystem : public Subsystem
	{
	public:
		WindowSubsystem() noexcept = default;
		~WindowSubsystem() noexcept override = default;

		WindowSubsystem(const WindowSubsystem&) = delete;
		WindowSubsystem& operator=(const WindowSubsystem&) = delete;

		WindowSubsystem(WindowSubsystem&&) = delete;
		WindowSubsystem& operator=(WindowSubsystem&&) = delete;

		void OnStartup() override;
		void OnShutdown() override;

		void OnPreTick();
		void OnPostTick();

		[[nodiscard]] std::expected<std::reference_wrapper<Window>, std::error_code> AddWindow(const WindowOptions& options);
		void RemoveWindow(const Window& window);

	private:
		std::vector<std::unique_ptr<Window>> windows;
	};
}
