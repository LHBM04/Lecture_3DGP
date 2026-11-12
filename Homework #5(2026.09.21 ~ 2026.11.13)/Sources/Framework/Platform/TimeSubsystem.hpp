#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "../Core/EngineSubsystem.hpp"

namespace TUK::Framework
{
	class App;

	class TimeSubsystem : public EngineSubsystem
	{
		friend class App;

	public:
		TimeSubsystem() noexcept;

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

		SystemStatus OnStartup() noexcept override;
		SystemStatus OnPreTick() noexcept override;
		SystemStatus OnTick() noexcept override;
		SystemStatus OnPostTick() noexcept override;
		void OnShutdown() noexcept override;

	private:
		[[nodiscard]] bool ShouldFixedStep() noexcept;

		LARGE_INTEGER startTime{};
		LARGE_INTEGER previousTime{};

		double totalTime = 0.0;
		double secondsPerCount = 0.0;

		double deltaTime = 0.0;
		float timeScale = 1.0f;

		double fixedDeltaTime = 1.0 / 60.0;
		double fixedAccumulator = 0.0;

		std::uint32_t maxFixedStepsPerFrame = 5;
		std::uint32_t fixedSteps = 0;
		std::uint32_t frameCount = 0;
	};
}
