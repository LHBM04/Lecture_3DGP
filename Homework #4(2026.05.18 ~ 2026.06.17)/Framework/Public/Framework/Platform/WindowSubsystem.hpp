#pragma once

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "Window.hpp"
#include "WindowOptions.hpp"

namespace TUK::Framework
{
	class WindowSubsystem : public EngineSubsystem
	{
	public:
		WindowSubsystem() noexcept;
		~WindowSubsystem() noexcept override;

		/** OnStartup 이후 호출. 크기는 창 전체 영역 기준. */
		std::expected<std::reference_wrapper<Window>, std::string> Create(const WindowOptions& options);

		/** 생성된 모든 창을 반환합니다. */
		[[nodiscard]] const std::vector<std::unique_ptr<Window>>& GetWindows() const noexcept;

	protected:
		void OnStartup() override;
		void PreTick() override;
		void PostTick() override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<Window>> windows;
	};
}

