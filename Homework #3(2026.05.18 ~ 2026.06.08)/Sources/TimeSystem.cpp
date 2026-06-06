#include "Precompiled.h"
#include "TimeSystem.h"

void TimeSystem::Reset()
{
	::QueryPerformanceFrequency(&frequency);
	::QueryPerformanceCounter(&lastTime);

	elapseTime = 0.0f;
	deltaTime = 0.0f;
	unscaledDeltaTime = 0.0f;
	fixedTime = 0.0f;
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

	const double seconds{
		static_cast<double>(currentTime.QuadPart - lastTime.QuadPart)
		/ static_cast<double>(frequency.QuadPart)
	};

	lastTime = currentTime;

	unscaledDeltaTime = std::min(static_cast<float>(seconds), maxDeltaTime);
	deltaTime = unscaledDeltaTime * timeScale;
	elapseTime += deltaTime;
	fixedTime += deltaTime;
}

float TimeSystem::GetTimeScale() const noexcept
{
	return timeScale;
}

void TimeSystem::SetTimeScale(float timeScale_) noexcept
{
	timeScale = std::max(0.0f, timeScale_);
}

float TimeSystem::GetElapseTime() const noexcept
{
	return elapseTime;
}

float TimeSystem::GetDeltaTime() const noexcept
{
	return deltaTime;
}

float TimeSystem::GetUnscaledDeltaTime() const noexcept
{
	return unscaledDeltaTime;
}

float TimeSystem::GetFixedTime() const noexcept
{
	return fixedTime;
}

float& TimeSystem::GetFixedTime() noexcept
{
	return fixedTime;
}

float TimeSystem::GetFixedDeltaTime() const noexcept
{
	return fixedStep;
}

float TimeSystem::GetUnscaledFixedDeltaTime() const noexcept
{
	return fixedStep;
}
