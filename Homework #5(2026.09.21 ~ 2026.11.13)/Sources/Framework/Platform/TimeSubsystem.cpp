#include "Precompiled.hpp"
#include "TimeSubsystem.hpp"

#include <cmath>

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
	{
	}

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
		if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0 ||
			!QueryPerformanceCounter(&startTime))
		{
			return SystemStatus::FAILURE;
		}

		secondsPerCount = 1.0 / static_cast<double>(frequency.QuadPart);

		previousTime = startTime;

		deltaTime = 0.0;
		totalTime = 0.0;

		frameCount = 0;

		fixedAccumulator = 0.0;
		fixedSteps = 0;

		return SystemStatus::CONTINUE;
	}

	SystemStatus TimeSubsystem::OnPreTick() noexcept
	{
		LARGE_INTEGER now{};
		if (!QueryPerformanceCounter(&now))
		{
			return SystemStatus::FAILURE;
		}

		deltaTime = (frameCount == 0 || now.QuadPart < previousTime.QuadPart) ? 0.0 : static_cast<double>(now.QuadPart - previousTime.QuadPart) * secondsPerCount;
		totalTime = static_cast<double>(now.QuadPart - startTime.QuadPart) * secondsPerCount;
		previousTime = now;

		++frameCount;

		if (timeScale > 0.0f)
		{
			fixedAccumulator += deltaTime;
		}
		fixedSteps = 0;

		return SystemStatus::CONTINUE;
	}

	SystemStatus TimeSubsystem::OnTick() noexcept
	{
		return SystemStatus::CONTINUE;
	}

	SystemStatus TimeSubsystem::OnPostTick() noexcept
	{
		return SystemStatus::CONTINUE;
	}

	void TimeSubsystem::OnShutdown() noexcept
	{
	}

	bool TimeSubsystem::ShouldFixedStep() noexcept
	{
		if (timeScale <= 0.0f)
		{
			return false;
		}

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
