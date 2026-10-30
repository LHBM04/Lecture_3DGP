#pragma once

#include <expected>
#include <functional>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

#include "Display.hpp"
#include "DisplayOptions.hpp"

namespace TUK::Framework
{
	class DisplaySubsystem : public EngineSubsystem
	{
		friend class System;

	public:
		using DisplayPtr = std::unique_ptr<Display>;
		using DisplayRef = std::reference_wrapper<Display>;
		using DisplayList = std::vector<DisplayPtr>;
		using DisplayCreateResult = std::expected<DisplayRef, std::string>;

		~DisplaySubsystem() noexcept override;

		DisplayCreateResult CreateDisplay(const DisplayOptions& options);

		[[nodiscard]] auto GetDisplays() noexcept;
		[[nodiscard]] const auto GetDisplays() const noexcept;

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		DisplaySubsystem() noexcept;

		DisplayList displays;
	};

	inline auto DisplaySubsystem::GetDisplays() noexcept
	{
		return displays | std::views::transform([](const DisplayPtr& displayPtr) -> DisplayRef
			{
				return *displayPtr;
			});
	}

	inline const auto DisplaySubsystem::GetDisplays() const noexcept
	{
		return displays | std::views::transform([](const DisplayPtr& displayPtr) -> DisplayRef
			{
				return *displayPtr;
			});
	}
}

