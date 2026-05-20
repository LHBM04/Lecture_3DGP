#pragma once

#include "Vector2D.hpp"
#include <chrono>

class Timer final
{
	STATIC_CLASS(Timer)
public:
	[[nodiscard]] static float GetTime();
	[[nodiscard]] static float GetUnscaledTime();

	[[nodiscard]] static float GetDeltaTime();
	[[nodiscard]] static float GetUnscaledDeltaTime();

	[[nodiscard]] static float GetFixedDeltaTime();
	[[nodiscard]] static float GetUnscaledFixedDeltaTime();

	[[nodiscard]] static int GetFrameCount();
	[[nodiscard]] static float GetFPS();

	static void Initialize();
	static void Update();

private:
	static float time;
	static float unscaledTime;

	static float deltaTime;
	static float unscaledDeltaTime;

	static float fixedDeltaTime;
	static float unscaledFixedDeltaTime;

	static float timeScale;

	static int frameCount;
	static int frameCounterInCurrentSecond;
	static float fps;
	static float fpsElapsedTime;

	static std::chrono::steady_clock::time_point lastTime;
	static bool isInitialized;
};
