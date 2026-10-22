#include "Precompiled.hpp"
#include "TimeSubsystem.hpp"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
		, startTime{}
		, previousTime{}
		, totalTime(0.0)
		, secondsPerCount(0.0)
		, deltaTime(0.0)
		, timeScale(1.0f)
		, fixedDeltaTime(1.0 / 60.0)
		, fixedAccumulator(0.0)
		, maxFixedStepsPerFrame(5)
		, fixedSteps(0)
		, frameCount(0)
	{
	}

	TimeSubsystem::~TimeSubsystem() noexcept = default;

	float TimeSubsystem::GetTotalTime() const noexcept
	{
		return static_cast<float>(totalTime);
	}

	float TimeSubsystem::GetTimeScale() const noexcept
	{
		return timeScale;
	}

	void TimeSubsystem::SetTimeScale(float timeScale) noexcept
	{
		this->timeScale = timeScale;
	}

	std::uint32_t TimeSubsystem::GetFixedStep() const noexcept
	{
		return maxFixedStepsPerFrame;
	}

	void TimeSubsystem::SetFixedStep(std::uint32_t count) noexcept
	{
		maxFixedStepsPerFrame = count;
	}

	float TimeSubsystem::GetDeltaTime() const noexcept
	{
		return static_cast<float>(deltaTime) * timeScale;
	}

	float TimeSubsystem::GetUnscaledDeltaTime() const noexcept
	{
		return static_cast<float>(deltaTime);
	}

	float TimeSubsystem::GetFixedDeltaTime() const noexcept
	{
		return static_cast<float>(fixedDeltaTime) * timeScale;
	}

	float TimeSubsystem::GetUnscaledFixedDeltaTime() const noexcept
	{
		return static_cast<float>(fixedDeltaTime);
	}

	std::uint32_t TimeSubsystem::GetFrameCount() const noexcept
	{
		return frameCount;
	}

	SystemStatus TimeSubsystem::OnStartup() noexcept
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

		return SystemStatus::Continue;
	}

	SystemStatus TimeSubsystem::OnPreTick() noexcept
	{
		LARGE_INTEGER now{};
		QueryPerformanceCounter(&now);

		deltaTime = (frameCount == 0) ? 0.0 : static_cast<double>(now.QuadPart - previousTime.QuadPart) * secondsPerCount;
		totalTime = static_cast<double>(now.QuadPart - startTime.QuadPart) * secondsPerCount;
		previousTime = now;

		++frameCount;

		fixedAccumulator += deltaTime * static_cast<double>(timeScale);
		fixedSteps = 0;

		return SystemStatus::Continue;
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
