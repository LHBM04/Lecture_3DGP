#include "Precompiled.h"
#include "TimeSubsystem.h"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: EngineSubsystem(0)
		, startTime()
		, previousTime()
		, secondsPerCount(0.0)
		, deltaTime(0.0)
		, elapsedTime(0.0)
		, fixedDeltaTime(1.0 / 60.0)
		, fixedAccumulator(0.0)
		, maxFixedStepsPerFrame(5)
		, fixedStepsThisFrame(0)
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

	double TimeSubsystem::GetElapsedTime() const noexcept
	{
		return elapsedTime;
	}

	std::uint64_t TimeSubsystem::GetFrameCount() const noexcept
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
		elapsedTime = 0.0;
		frameCount = 0;
		ResetFixedSteps();
	}

	void TimeSubsystem::OnPreTick() noexcept
	{
		LARGE_INTEGER now{};
		QueryPerformanceCounter(&now);
		// 초기화에 걸린 시간이 첫 프레임의 이동량에 반영되지 않도록 한다.
		deltaTime = frameCount == 0 ? 0.0 : static_cast<double>(now.QuadPart - previousTime.QuadPart) * secondsPerCount;
		elapsedTime = static_cast<double>(now.QuadPart - startTime.QuadPart) * secondsPerCount;
		previousTime = now;
		++frameCount;
		fixedAccumulator += deltaTime;
		fixedStepsThisFrame = 0;
	}
	float TimeSubsystem::GetUnscaledDeltaTime() const noexcept
	{
		// 시간 배율을 도입하기 전까지는 실제 경과 시간과 같다.
		return static_cast<float>(deltaTime);
	}

	double TimeSubsystem::GetFixedDeltaTime() const noexcept
	{
		return fixedDeltaTime;
	}

	void TimeSubsystem::SetFixedDeltaTime(double seconds) noexcept
	{
		assert(std::isfinite(seconds) && seconds > 0.0);
		fixedDeltaTime = seconds;
	}

	std::uint32_t TimeSubsystem::GetMaxFixedStepsPerFrame() const noexcept
	{
		return maxFixedStepsPerFrame;
	}

	void TimeSubsystem::SetMaxFixedStepsPerFrame(std::uint32_t count) noexcept
	{
		assert(count > 0);
		maxFixedStepsPerFrame = count;
	}

	bool TimeSubsystem::ConsumeFixedStep() noexcept
	{
		if (fixedStepsThisFrame >= maxFixedStepsPerFrame)
		{
			// 초과한 전체 스텝은 버리고 다음 프레임에 사용할 나머지만 남긴다.
			fixedAccumulator = std::fmod(fixedAccumulator, fixedDeltaTime);
			return false;
		}

		if (fixedAccumulator < fixedDeltaTime)
		{
			return false;
		}

		fixedAccumulator -= fixedDeltaTime;
		++fixedStepsThisFrame;
		return true;
	}

	void TimeSubsystem::ResetFixedSteps() noexcept
	{
		fixedAccumulator = 0.0;
		fixedStepsThisFrame = 0;
	}
}
