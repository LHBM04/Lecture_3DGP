#include "Precompiled.hpp"
#include "Timer.hpp"

float Timer::GetTime()
{
	return time;
}

float Timer::GetUnscaledTime()
{
	return unscaledTime;
}

float Timer::GetDeltaTime()
{
	return deltaTime;
}

float Timer::GetUnscaledDeltaTime()
{
	return unscaledDeltaTime;
}

float Timer::GetFixedDeltaTime()
{
	return fixedDeltaTime;
}

float Timer::GetUnscaledFixedDeltaTime()
{
	return unscaledFixedDeltaTime;
}

int Timer::GetFrameCount()
{
	return frameCount;
}

float Timer::GetFPS()
{
	return fps;
}

void Timer::Initialize()
{
	lastTime = std::chrono::steady_clock::now();

	time = 0.0f;
	unscaledTime = 0.0f;

	deltaTime = 0.0f;
	unscaledDeltaTime = 0.0f;

	unscaledFixedDeltaTime = 1.0f / 60.0f;
	fixedDeltaTime = unscaledFixedDeltaTime;
	timeScale = 1.0f;

	frameCount = 0;
	frameCounterInCurrentSecond = 0;
	fps = 0.0f;
	fpsElapsedTime = 0.0f;
	isInitialized = true;
}

void Timer::Update()
{
	if (!isInitialized)
	{
		Initialize();
	}

	const auto now = std::chrono::steady_clock::now();
	const auto elapsed = now - lastTime;
	lastTime = now;

	unscaledDeltaTime = std::chrono::duration<float>(elapsed).count();

	deltaTime = unscaledDeltaTime * timeScale;

	unscaledTime += unscaledDeltaTime;
	time += deltaTime;

	fixedDeltaTime = unscaledFixedDeltaTime * timeScale;

	++frameCount;
	++frameCounterInCurrentSecond;

	fpsElapsedTime += unscaledDeltaTime;
	if (fpsElapsedTime >= 1.0f)
	{
		fps = static_cast<float>(frameCounterInCurrentSecond) / fpsElapsedTime;
		frameCounterInCurrentSecond = 0;
		fpsElapsedTime = 0.0f;
	}
}

float Timer::time = 0.0f;
float Timer::unscaledTime = 0.0f;

float Timer::deltaTime = 0.0f;
float Timer::unscaledDeltaTime = 0.0f;

float Timer::fixedDeltaTime = 1.0f / 60.0f;
float Timer::unscaledFixedDeltaTime = 1.0f / 60.0f;

float Timer::timeScale = 1.0f;

int Timer::frameCount = 0;
int Timer::frameCounterInCurrentSecond = 0;
float Timer::fps = 0.0f;
float Timer::fpsElapsedTime = 0.0f;

std::chrono::steady_clock::time_point Timer::lastTime{};

bool Timer::isInitialized = false;
