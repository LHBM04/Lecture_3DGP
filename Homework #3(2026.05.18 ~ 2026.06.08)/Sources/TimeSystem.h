#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Singleton.h"

class TimeSystem : public Singleton<TimeSystem>
{
public:
	TimeSystem() noexcept = default;
	~TimeSystem() noexcept override = default;

	void Reset();
	void Tick();

	[[nodiscard]] float GetTimeScale() const noexcept;
	void SetTimeScale(float timeScale_) noexcept;

	[[nodiscard]] float GetElapseTime() const noexcept;

	[[nodiscard]] float GetDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

	[[nodiscard]] float GetFixedTime() const noexcept;
	[[nodiscard]] float& GetFixedTime() noexcept;

	[[nodiscard]] float GetFixedDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;

private:
	static constexpr float fixedStep{ 1.0f / 60.0f };
	static constexpr float maxDeltaTime{ 0.25f };

	LARGE_INTEGER frequency{};
	LARGE_INTEGER lastTime{};

	float timeScale{ 1.0f };
	float elapseTime{ 0.0f };
	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };
	float fixedTime{ 0.0f };
};
