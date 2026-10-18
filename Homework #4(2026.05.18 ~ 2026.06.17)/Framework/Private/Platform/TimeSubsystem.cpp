#include "Precompiled.hpp"
#include "Framework/Platform/TimeSubsystem.hpp"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
		, totalTime(0.0)
		, secondsPerCount(0.0)
		, deltaTime(0.0)
		, fixedDeltaTime(1.0 / 60.0)
		, fixedAccumulator(0.0)
		, maxFixedStepsPerFrame(5)
		, fixedSteps(0)
		, frameCount(0)
	{
	}

	TimeSubsystem::~TimeSubsystem() noexcept
	{
	}

	float TimeSubsystem::GetDeltaTime() const noexcept
	{
		return static_cast<float>(deltaTime);
	}

	float TimeSubsystem::GetTotalTime() const noexcept
	{
		return static_cast<float>(totalTime);
	}

	std::uint32_t TimeSubsystem::GetFrameCount() const noexcept
	{
		return frameCount;
	}

	SystemStatus TimeSubsystem::OnStartup() noexcept
	{
		return SystemStatus::Continue;
	}

	SystemStatus TimeSubsystem::OnPreTick() noexcept
	{
		return SystemStatus::Continue;
	}

	float TimeSubsystem::GetUnscaledDeltaTime() const noexcept
	{
		return static_cast<float>(deltaTime);
	}

	float TimeSubsystem::GetFixedDeltaTime() const noexcept
	{
		return static_cast<float>(fixedDeltaTime);
	}

	float TimeSubsystem::GetUnscaledFixedDeltaTime() const noexcept
	{
		return static_cast<float>(fixedDeltaTime);
	}

	std::uint32_t TimeSubsystem::GetFixedStep() const noexcept
	{
		return maxFixedStepsPerFrame;
	}

	void TimeSubsystem::SetFixedStep(std::uint32_t count) noexcept
	{
		maxFixedStepsPerFrame = count;
	}

	bool TimeSubsystem::ShouldFixedStep() noexcept
	{
		if (fixedSteps >= maxFixedStepsPerFrame)
		{
			fixedAccumulator = std::fmod(fixedAccumulator, fixedDeltaTime);
			return false;
		}

		if (fixedAccumulator < fixedDeltaTime)
		{
			return false;
		}

		fixedAccumulator -= fixedDeltaTime;
		++fixedSteps;

		return true;
	}
}
