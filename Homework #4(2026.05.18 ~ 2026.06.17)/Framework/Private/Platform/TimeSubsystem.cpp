#include "Precompiled.hpp"
#include "TimeSubsystem.hpp"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
		, startTime()
		, previousTime()
		, secondsPerCount(0.0)
		, deltaTime(0.0)
		, totalTime(0.0)
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

	unsigned long TimeSubsystem::GetFrameCount() const noexcept
	{
		return frameCount;
	}

	void TimeSubsystem::OnStartup() noexcept
	{
		LARGE_INTEGER frequency{};
		QueryPerformanceFrequency(&frequency);

		secondsPerCount = 1.0 / static_cast<double>(frequency.QuadPart);
		
		QueryPerformanceCounter(&startTime);
		previousTime = startTime;
		
		deltaTime = 0.0;
		totalTime = 0.0;

		frameCount = 0;
		
		fixedAccumulator = 0.0;
		fixedSteps = 0;
	}

	void TimeSubsystem::PreTick() noexcept
	{
		LARGE_INTEGER now{};
		QueryPerformanceCounter(&now);

		deltaTime = frameCount == 0 ? 0.0 : static_cast<double>(now.QuadPart - previousTime.QuadPart) * secondsPerCount;
		totalTime = static_cast<double>(now.QuadPart - startTime.QuadPart) * secondsPerCount;
		previousTime = now;

		++frameCount;

		fixedAccumulator += deltaTime;
		fixedSteps = 0;
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

	unsigned int TimeSubsystem::GetFixedStep() const noexcept
	{
		return maxFixedStepsPerFrame;
	}

	void TimeSubsystem::SetFixedStep(unsigned int count) noexcept
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
