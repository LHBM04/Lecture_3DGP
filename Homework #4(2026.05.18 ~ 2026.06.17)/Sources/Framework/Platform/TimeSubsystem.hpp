#pragma once

#include <chrono>
#include <cstdint>

#include "Framework/Core/EngineSubsystem.hpp"

namespace TUK::Framework
{
	class TimeSubsystem : public EngineSubsystem
	{
		friend class Game;

	public:
		TimeSubsystem() noexcept;
		~TimeSubsystem() noexcept override;

		[[nodiscard]] float GetTotalTime() const noexcept;

		[[nodiscard]] float GetTimeScale() const noexcept;
		void SetTimeScale(float timeScale) noexcept;

		[[nodiscard]] std::uint32_t GetFixedStep() const noexcept;
		void SetFixedStep(std::uint32_t count) noexcept;

		[[nodiscard]] float GetDeltaTime() const noexcept;
		[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

		[[nodiscard]] float GetFixedDeltaTime() const noexcept;
		[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;

		[[nodiscard]] std::uint32_t GetFrameCount() const noexcept;

	protected:
		SystemStatus OnStartup() noexcept override;
		SystemStatus OnPreTick() noexcept override;

	private:
		[[nodiscard]] bool ShouldFixedStep() noexcept;

		double totalTime;
		double secondsPerCount;

		double deltaTime;

		double fixedDeltaTime;
		double fixedAccumulator;

		std::uint32_t maxFixedStepsPerFrame;
		std::uint32_t fixedSteps;
		std::uint32_t frameCount;
	};
}
