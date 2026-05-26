#pragma once

namespace Timer
{
	void Reset();
	void Tick();

	[[nodiscard]] float GetTotalTime() noexcept;

	[[nodiscard]] float GetDeltaTime() noexcept;
	[[nodiscard]] float GetUnscaledDeltaTime() noexcept;
	[[nodiscard]] float GetUnscaledTime() noexcept;

	[[nodiscard]] float GetFixedDeltaTime() noexcept;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() noexcept;
	void SetFixedDeltaTime(float fixedDeltaTime_) noexcept;

	[[nodiscard]] int GetTotalFrameCount() noexcept;
	[[nodiscard]] int GetFrameCount() noexcept;

	[[nodiscard]] float GetTimeScale() noexcept;
	void SetTimeScale(float timeScale_) noexcept;

	[[nodiscard]] int GetFps() noexcept;
}
