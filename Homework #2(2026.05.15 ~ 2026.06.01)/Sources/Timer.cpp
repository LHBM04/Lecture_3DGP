#include "Precompiled.h"
#include "Timer.h"

#include <algorithm>

void Timer::Initialize() noexcept
{
	startTime = Clock::now();
	lastTime = startTime;
}

void Timer::Tick() noexcept
{
	const TimePoint currentTime{ Clock::now() };
	const std::chrono::duration<float> duration{ currentTime - lastTime };
	
	unscaledDeltaTime = duration.count();
	deltaTime = unscaledDeltaTime * timeScale;
	
	lastTime = currentTime;

	unscaledTime = std::chrono::duration<float>(currentTime - startTime).count();
	totalTime += deltaTime;

	++frameCount;
	++totalFrameCount;
	fpsTimer += unscaledDeltaTime;

	if (fpsTimer >= 1.0f)
	{
		currentFps = frameCount;
		frameCount = 0;
		fpsTimer -= 1.0f;
	}
}

float Timer::GetTotalTime() const noexcept
{
	return totalTime;
}

float Timer::GetDeltaTime() const noexcept
{
	return deltaTime;
}

float Timer::GetUnscaledDeltaTime() const noexcept
{
	return unscaledDeltaTime;
}

float Timer::GetUnscaledTime() const noexcept
{
	return unscaledTime;
}

float Timer::GetFixedDeltaTime() const noexcept
{
	return fixedDeltaTime * timeScale;
}

float Timer::GetUnscaledFixedDeltaTime() const noexcept
{
	return fixedDeltaTime;
}

void Timer::SetFixedDeltaTime(float fixedDeltaTime_) noexcept
{
	fixedDeltaTime = std::max(fixedDeltaTime_, 0.0f);
}

int Timer::GetFrameCount() const noexcept
{
	return totalFrameCount;
}

int Timer::GetFPS() const noexcept
{
	return currentFps;
}

float Timer::GetTimeScale() const noexcept
{
	return timeScale;
}

void Timer::SetTimeScale(float timeScale_) noexcept
{
	timeScale = std::max(timeScale_, 0.0f);
}
