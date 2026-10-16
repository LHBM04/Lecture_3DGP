#pragma once

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "Display.hpp"
#include "DisplayOptions.hpp"

namespace TUK::Framework
{
	class DisplaySubsystem : public EngineSubsystem
	{
	public:
		DisplaySubsystem() noexcept;
		~DisplaySubsystem() noexcept override;

		/** OnStartup 이후 호출. 크기는 창 전체 영역 기준. */
		std::expected<std::reference_wrapper<Display>, std::string> Create(const DisplayOptions& options);

		/** 생성된 모든 창을 반환합니다. */
		[[nodiscard]] const std::vector<std::unique_ptr<Display>>& GetWindows() const noexcept;

	protected:
		void OnStartup() override;
		void PreTick() override;
		void PostTick() override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<Display>> windows;
	};
}

