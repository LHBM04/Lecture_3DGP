#include "Precompiled.h"
#include "Timer.h"

namespace
{
	float totalTime{ 0.0f };

	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };
	
	float fixedDeltaTime{ 0.02f };
	float unscaledFixedDeltaTime{ 0.02f };
	
	float timeScale{ 1.0f };

	LONGLONG frequency{ 0 };
	LONGLONG startTick{ 0 };
	LONGLONG previousTick{ 0 };
}

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
