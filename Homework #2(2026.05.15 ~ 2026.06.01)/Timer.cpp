#include "Precompiled.h"
#include "Timer.h"

void Timer::Initialize()
{
	LARGE_INTEGER freq;
	if (QueryPerformanceFrequency(&freq))
	{
		frequency = freq.QuadPart;
	}

	LARGE_INTEGER count;
	if (QueryPerformanceCounter(&count))
	{
		startTick = count.QuadPart;
		previousTick = startTick;
	}

	totalTime = 0.0f;
	deltaTime = 0.0f;
	unscaledDeltaTime = 0.0f;
	timeScale = 1.0f;
}

void Timer::Tick()
{
	LARGE_INTEGER count;
	if (QueryPerformanceCounter(&count))
	{
		int64_t currentTick = count.QuadPart;
		
		unscaledDeltaTime = static_cast<float>(currentTick - previousTick) / static_cast<float>(frequency);
		deltaTime = unscaledDeltaTime * timeScale;
		totalTime = static_cast<float>(currentTick - startTick) / static_cast<float>(frequency);

		previousTick = currentTick;
	}
}

float Timer::GetTotalTime() noexcept
{
	return totalTime;
}

float Timer::GetDeltaTime() noexcept
{
	return deltaTime;
}

float Timer::GetUnscaledDeltaTime() noexcept
{
	return unscaledDeltaTime;
}

float Timer::GetFixedDeltaTime() noexcept
{
	return fixedDeltaTime;
}

float Timer::GetUnscaledFixedDeltaTime() noexcept
{
	return unscaledFixedDeltaTime;
}

float Timer::GetTimeScale() noexcept
{
	return timeScale;
}

void Timer::SetTimeScale(float timeScale_) noexcept
{
	timeScale = std::max(timeScale_, 0.0f);
}
