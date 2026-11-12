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

		[[nodiscard]] f32 GetTotalTime() const noexcept;

		[[nodiscard]] f32 GetTimeScale() const noexcept;
		void SetTimeScale(f32 value) noexcept;

		[[nodiscard]] u32 GetFixedStep() const noexcept;
		void SetFixedStep(u32 value) noexcept;

		[[nodiscard]] f32 GetDeltaTime() const noexcept;
		[[nodiscard]] f32 GetUnscaledDeltaTime() const noexcept;

		[[nodiscard]] f32 GetFixedDeltaTime() const noexcept;
		[[nodiscard]] f32 GetUnscaledFixedDeltaTime() const noexcept;

		[[nodiscard]] u32 GetFrameCount() const noexcept;

		SystemStatus OnStartup() noexcept override;
		SystemStatus OnPreTick() noexcept override;
		SystemStatus OnTick() noexcept override;
		SystemStatus OnPostTick() noexcept override;
		void OnShutdown() noexcept override;

	private:
		[[nodiscard]] bool ShouldFixedStep() noexcept;

		LARGE_INTEGER startTime{};
		LARGE_INTEGER previousTime{};

		f64 totalTime = 0.0;
		f64 secondsPerCount = 0.0;

		f64 deltaTime = 0.0;
		f32 timeScale = 1.0f;

		f64 fixedDeltaTime = 1.0 / 60.0;
		f64 fixedAccumulator = 0.0;

		u32 maxFixedStepsPerFrame = 5;
		u32 fixedSteps = 0;
		u32 frameCount = 0;
	};
}
