#pragma once

#include "../Core/Subsystem.h"

namespace TUK::Framework
{
	struct WindowOptions;
	class Window;

	class WindowSubsystem : public Subsystem
	{
	public:
		WindowSubsystem() noexcept;
		~WindowSubsystem() noexcept override;

		WindowSubsystem(const WindowSubsystem&) = delete;
		WindowSubsystem& operator=(const WindowSubsystem&) = delete;

		WindowSubsystem(WindowSubsystem&&) = delete;
		WindowSubsystem& operator=(WindowSubsystem&&) = delete;

		[[nodiscard]] Window* AddWindow(const WindowOptions& options);
		void RemoveWindow(Window* window);
	};
}
