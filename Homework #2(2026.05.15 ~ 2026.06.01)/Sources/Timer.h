#pragma once

#include "Singleton.h"
#include <chrono>

class Timer final : public Singleton<Timer>
{
public:
	void Initialize() noexcept;
	void Tick() noexcept;

	[[nodiscard]] float GetTotalTime() const noexcept;

	[[nodiscard]] float GetDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledTime() const noexcept;

	[[nodiscard]] float GetFixedDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;
	void SetFixedDeltaTime(float fixedDeltaTime_) noexcept;

	[[nodiscard]] int GetFrameCount() const noexcept;
	[[nodiscard]] int GetFPS() const noexcept;

	[[nodiscard]] float GetTimeScale() const noexcept;
	void SetTimeScale(float timeScale_) noexcept;

private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint startTime{};
	TimePoint lastTime{};

	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };

	float totalTime{ 0.0f };
	float unscaledTime{ 0.0f };

	float fixedDeltaTime{ 0.02f };
	float timeScale{ 1.0f };

	int frameCount{ 0 };
	int totalFrameCount{ 0 };

	float fpsTimer{ 0.0f };
	int currentFps{ 0 };
};
