#include "Precompiled.h"
#include "TimeSubsystem.h"

namespace TUK::Framework
{
	TimeSubsystem::TimeSubsystem() noexcept
		: Subsystem(0)
		, startTime()
		, previousTime()
		, secondsPerCount(0.0)
		, deltaTime(0.0)
		, elapsedTime(0.0)
		, frameCount(0)
	{
	}

	TimeSubsystem::~TimeSubsystem() noexcept = default;

	double TimeSubsystem::GetDeltaTime() const noexcept
	{
		return deltaTime;
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
	}
}
