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
		using DisplayPtr = std::unique_ptr<Display>;
		using DisplayRef = std::reference_wrapper<Display>;
		using DisplayList = std::vector<DisplayPtr>;
		using DisplayCreateResult = std::expected<DisplayRef, std::string>;

		DisplaySubsystem() noexcept;
		~DisplaySubsystem() noexcept override;

		/** OnStartup 이후 호출. 크기는 창 전체 영역 기준. */
		DisplayCreateResult CreateDisplay(const DisplayOptions& options);

		/** 생성된 모든 창을 반환합니다. */
		[[nodiscard]] const DisplayList& GetDisplays() const noexcept;

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		DisplayList displays;
	};
}

