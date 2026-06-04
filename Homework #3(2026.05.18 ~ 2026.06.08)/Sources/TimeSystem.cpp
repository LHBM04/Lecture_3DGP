#include "Precompiled.h"
#include "TimeSystem.h"

void TimeSystem::Reset()
{
	::QueryPerformanceFrequency(&frequency);
	::QueryPerformanceCounter(&lastTime);

	elapsedTime = 0.0f;
	deltaTime = 0.0f;
	unscaledDeltaTime = 0.0f;
	fixedTimeAccumulator = 0.0f;
}

void TimeSystem::Tick()
{
	if (frequency.QuadPart == 0)
	{
		Reset();
		return;
	}

	LARGE_INTEGER currentTime{};
	::QueryPerformanceCounter(&currentTime);

	const LONGLONG elapsedCounts{ currentTime.QuadPart - lastTime.QuadPart };
	lastTime = currentTime;

	const double elapsedSeconds{
		static_cast<double>(elapsedCounts) / static_cast<double>(frequency.QuadPart)
	};

	unscaledDeltaTime = static_cast<float>(std::max(0.0, elapsedSeconds));
	deltaTime = unscaledDeltaTime * timeScale;

	elapsedTime += deltaTime;
	fixedTimeAccumulator += deltaTime;
}

bool TimeSystem::ConsumeFixedUpdate() noexcept
{
	if (fixedDeltaTime <= 0.0f || fixedTimeAccumulator < fixedDeltaTime)
	{
		return false;
	}

	fixedTimeAccumulator -= fixedDeltaTime;
	return true;
}

float TimeSystem::GetElapseTime() const noexcept
{
	return elapsedTime;
}

float TimeSystem::GetTimeScale() const noexcept
{
	return timeScale;
}

void TimeSystem::SetTimeScale(float timeScale_) noexcept
{
	timeScale = std::max(0.0f, timeScale_);
	fixedDeltaTime = unscaledFixedDeltaTime * timeScale;
}

float TimeSystem::GetDeltaTime() const noexcept
{
	return deltaTime;
}

float TimeSystem::GetUnscaledDeltaTime() const noexcept
{
	return unscaledDeltaTime;
}

float TimeSystem::GetFixedDeltaTime() const noexcept
{
	return fixedDeltaTime;
}

float TimeSystem::GetUnscaledFixedDeltaTime() const noexcept
{
	return unscaledFixedDeltaTime;
}
