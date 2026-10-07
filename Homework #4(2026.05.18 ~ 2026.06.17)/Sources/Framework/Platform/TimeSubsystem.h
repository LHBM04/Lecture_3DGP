#pragma once

#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Core/EngineSubsystem.h"

namespace TUK::Framework
{
	class TimeSubsystem : public EngineSubsystem
	{
	public:
		TimeSubsystem() noexcept;
		~TimeSubsystem() noexcept override;

		/** 이전 프레임 시작부터 현재 프레임 시작까지의 시간(초). 첫 프레임은 0. */
		[[nodiscard]] float GetDeltaTime() const noexcept;
		[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

		[[nodiscard]] double GetFixedDeltaTime() const noexcept;
		void SetFixedDeltaTime(double seconds) noexcept;
		[[nodiscard]] std::uint32_t GetMaxFixedStepsPerFrame() const noexcept;
		void SetMaxFixedStepsPerFrame(std::uint32_t count) noexcept;

		/** 누적 시간은 한 Game에서만 소비한다. */
		[[nodiscard]] bool ConsumeFixedStep() noexcept;
		void ResetFixedSteps() noexcept;

		/** OnStartup부터 현재 프레임 시작까지의 누적 시간(초). */
		[[nodiscard]] double GetElapsedTime() const noexcept;
		/** 시작된 프레임 수. 첫 프레임에서 1. */
		[[nodiscard]] std::uint64_t GetFrameCount() const noexcept;

	protected:
		void OnStartup() noexcept override;
		void OnPreTick() noexcept override;

	private:
		LARGE_INTEGER startTime;
		LARGE_INTEGER previousTime;
		double secondsPerCount;
		double deltaTime;
		double elapsedTime;
		double fixedDeltaTime;
		double fixedAccumulator;
		std::uint32_t maxFixedStepsPerFrame;
		std::uint32_t fixedStepsThisFrame;
		std::uint64_t frameCount;
	};
}
