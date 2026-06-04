#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "Singleton.h"

class TimeSystem final : public Singleton<TimeSystem>
{
public:
	TimeSystem() = default;
	~TimeSystem() override = default;

	void Reset();
	void Tick();
	[[nodiscard]] bool ConsumeFixedUpdate() noexcept;

	[[nodiscard]] float GetElapseTime() const noexcept;

	[[nodiscard]] float GetTimeScale() const noexcept;
	void SetTimeScale(float timeScale_) noexcept;

	[[nodiscard]] float GetDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

	[[nodiscard]] float GetFixedDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;

private:
	LARGE_INTEGER frequency{};
	LARGE_INTEGER lastTime{};

	float elapsedTime{ 0.0f };
	float timeScale{ 1.0f };

	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };

	float fixedDeltaTime{ 0.02f };
	float unscaledFixedDeltaTime{ 0.02f };
	float fixedTimeAccumulator{ 0.0f };
};
