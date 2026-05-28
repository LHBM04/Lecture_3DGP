#include "Precompiled.h"
#include "TimeSystem.h"

bool TimeSystem::Initialize()
{
	QueryPerformanceFrequency(&frequency);
	Reset();

    return true;
}

void TimeSystem::Release()
{
}

void TimeSystem::Reset()
{
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&startTime);
    lastTime = startTime;
}

void TimeSystem::Tick()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    LARGE_INTEGER elapsedTicks;
    elapsedTicks.QuadPart = currentTime.QuadPart - lastTime.QuadPart;

    unscaledDeltaTime = static_cast<float>(elapsedTicks.QuadPart) / static_cast<float>(frequency.QuadPart);
    deltaTime = unscaledDeltaTime * timeScale;

    lastTime = currentTime;

    LARGE_INTEGER totalTicks;
    totalTicks.QuadPart = currentTime.QuadPart - startTime.QuadPart;

    unscaledTime = static_cast<float>(totalTicks.QuadPart) / static_cast<float>(frequency.QuadPart);
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

float TimeSystem::GetTotalTime() const
{
    return 0.0f;
}

float TimeSystem::GetTimeScale() const
{
    return 0.0f;
}

float TimeSystem::GetDeltaTime() const
{
    return 0.0f;
}

float TimeSystem::GetUnscaledDeltaTime() const
{
    return 0.0f;
}

float TimeSystem::GetFixedDeltaTime() const
{
    return 0.0f;
}

float TimeSystem::GetUnscaledFixedDeltaTime() const
{
    return 0.0f;
}

float TimeSystem::GetFrameCount() const
{
    return 0.0f;
}
