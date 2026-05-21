#pragma once

#include <cstdint>

class Timer final
{
	STATIC_CLASS(Timer);

public:
	static void Initialize();
	static void Tick();

	[[nodiscard]] static float GetTotalTime() noexcept;
	[[nodiscard]] static float GetDeltaTime() noexcept;
	[[nodiscard]] static float GetUnscaledDeltaTime() noexcept;
	[[nodiscard]] static float GetFixedDeltaTime() noexcept;
	[[nodiscard]] static float GetUnscaledFixedDeltaTime() noexcept;
	[[nodiscard]] static float GetTimeScale() noexcept;
	static void SetTimeScale(float timeScale_) noexcept;

private:
	static inline float totalTime{ 0.0f };
	static inline float deltaTime{ 0.0f };
	static inline float unscaledDeltaTime{ 0.0f };
	static inline float fixedDeltaTime{ 0.02f };
	static inline float unscaledFixedDeltaTime{ 0.02f };
	static inline float timeScale{ 1.0f };

	static inline int64_t frequency{ 0 };
	static inline int64_t startTick{ 0 };
	static inline int64_t previousTick{ 0 };
};
