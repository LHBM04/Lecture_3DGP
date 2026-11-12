#include "Precompiled.hpp"
#include "TimeSubsystem.hpp"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
	{
	}

	f32 TimeSubsystem::GetTotalTime() const noexcept
	{
		return static_cast<f32>(totalTime);
	}

	f32 TimeSubsystem::GetTimeScale() const noexcept
	{
		return timeScale;
	}

	void TimeSubsystem::SetTimeScale(f32 value) noexcept
	{
		timeScale = value;
	}

	u32 TimeSubsystem::GetFixedStep() const noexcept
	{
		return maxFixedStepsPerFrame;
	}

	void TimeSubsystem::SetFixedStep(u32 value) noexcept
	{
		maxFixedStepsPerFrame = value;
	}

	f32 TimeSubsystem::GetDeltaTime() const noexcept
	{
		return static_cast<f32>(deltaTime) * timeScale;
	}

	f32 TimeSubsystem::GetUnscaledDeltaTime() const noexcept
	{
		return static_cast<f32>(deltaTime);
	}

	f32 TimeSubsystem::GetFixedDeltaTime() const noexcept
	{
		return static_cast<f32>(fixedDeltaTime) * timeScale;
	}

	f32 TimeSubsystem::GetUnscaledFixedDeltaTime() const noexcept
	{
		return static_cast<f32>(fixedDeltaTime);
	}

	u32 TimeSubsystem::GetFrameCount() const noexcept
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

		secondsPerCount = 1.0 / static_cast<f64>(frequency.QuadPart);

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

		deltaTime = (frameCount == 0 || now.QuadPart < previousTime.QuadPart) ? 0.0 : static_cast<f64>(now.QuadPart - previousTime.QuadPart) * secondsPerCount;
		totalTime = static_cast<f64>(now.QuadPart - startTime.QuadPart) * secondsPerCount;
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
