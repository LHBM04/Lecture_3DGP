#ifndef HOMEWORK04_FRAMEWORK_PLATFORM_WINDOW_SUBSYSTEM
#define HOMEWORK04_FRAMEWORK_PLATFORM_WINDOW_SUBSYSTEM

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../Core/Subsystem.h"
#include "Window.h"
#include "WindowOptions.h"

namespace TUK::Framework
{
	class WindowSubsystem : public Subsystem
	{
	public:
		WindowSubsystem() noexcept;
		~WindowSubsystem() noexcept override;

		/** OnStartup 이후 호출. 크기는 창 전체 영역 기준. */
		std::expected<std::reference_wrapper<Window>, std::string> Create(const WindowOptions& options);

		[[nodiscard]] const std::vector<std::unique_ptr<Window>>& GetWindows() const noexcept;

	protected:
		void OnStartup() override;
		void OnPreTick() override;
		void OnPostTick() override;
		void OnShutdown() override;

	private:
		ATOM windowClass;
		bool hasExitRequest;
		std::vector<std::unique_ptr<Window>> windows;
	};
}

#endif
