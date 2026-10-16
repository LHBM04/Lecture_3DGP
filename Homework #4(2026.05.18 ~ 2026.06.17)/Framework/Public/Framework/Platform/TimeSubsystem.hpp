#pragma once

#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Core/EngineSubsystem.hpp"

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

		[[nodiscard]] unsigned int GetFixedStep() const noexcept;
		void SetFixedStep(unsigned int count) noexcept;

		[[nodiscard]] float GetDeltaTime() const noexcept;
		[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

		[[nodiscard]] float GetFixedDeltaTime() const noexcept;
		[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;

		[[nodiscard]] unsigned long GetFrameCount() const noexcept;

	protected:
		void OnStartup() noexcept override;
		void PreTick() noexcept override;

	private:
		[[nodiscard]] bool ShouldFixedStep() noexcept;

		LARGE_INTEGER startTime;
		LARGE_INTEGER previousTime;

		double totalTime;
		double secondsPerCount;

		double deltaTime;

		double fixedDeltaTime;
		double fixedAccumulator;

		unsigned int maxFixedStepsPerFrame;
		unsigned int fixedSteps;
		unsigned long frameCount;
	};
}
