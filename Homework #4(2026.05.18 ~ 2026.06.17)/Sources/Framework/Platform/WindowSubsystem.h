#pragma once

#include <expected>
#include <string>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../Core/EngineSubsystem.h"
#include "Window.h"
#include "WindowOptions.h"

namespace TUK::Framework
{
	class WindowSubsystem : public EngineSubsystem
	{
	public:
		WindowSubsystem() noexcept;
		~WindowSubsystem() noexcept override;

		/** OnStartup 이후 호출. 크기는 창 전체 영역 기준. */
		std::expected<std::reference_wrapper<Window>, std::string> Create(const WindowOptions& options);

		[[nodiscard]] const std::vector<std::unique_ptr<Window>>& GetWindows() const noexcept;

	protected:
		void OnStartup() override;
		void OnPostTick() override;
		void OnShutdown() override;

	private:
		ATOM windowClass;
		bool hasExitRequest;
		std::vector<std::unique_ptr<Window>> windows;
	};
}

