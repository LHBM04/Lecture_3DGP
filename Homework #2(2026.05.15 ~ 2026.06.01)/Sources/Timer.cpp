#include "Precompiled.h"
#include "Timer.h"

namespace
{
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint startTime{};
	TimePoint lastTime{};

	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };

	float totalTime{ 0.0f };
	float unscaledTime{ 0.0f };

	float fixedDeltaTime{ 1.0f / 60.0f };
	float timeScale{ 1.0f };

	int frameCount{ 0 };
	int totalFrameCount{ 0 };

	float fpsTimer{ 0.0f };
	int currentFps{ 0 };
}

void Timer::Reset()
{
	startTime = Clock::now();
	lastTime = startTime;
}

void Timer::Tick()
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

float Timer::GetUnscaledTime() noexcept
{
	return unscaledTime;
}

float Timer::GetFixedDeltaTime() noexcept
{
	return fixedDeltaTime * timeScale;
}

float Timer::GetUnscaledFixedDeltaTime() noexcept
{
	return fixedDeltaTime;
}

void Timer::SetFixedDeltaTime(float fixedDeltaTime_) noexcept
{
	fixedDeltaTime = std::max(fixedDeltaTime_, 0.0f);
}

int Timer::GetTotalFrameCount() noexcept 
{
	return totalFrameCount;
}

int Timer::GetFrameCount() noexcept
{
	return frameCount;
}

float Timer::GetTimeScale() noexcept
{
	return timeScale;
}

void Timer::SetTimeScale(float timeScale_) noexcept
{
	timeScale = std::max(timeScale_, 0.0f);
}

int Timer::GetFps() noexcept
{
	return currentFps;
}
