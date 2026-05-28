#pragma once

class TimeSystem;

class TimeContext
{
public:
	TimeContext() = default;
	explicit TimeContext(const TimeSystem* timeSystem_) noexcept;

	[[nodiscard]] float GetTotalTime() const;

	[[nodiscard]] float GetTimeScale() const;

	[[nodiscard]] float GetDeltaTime() const;
	[[nodiscard]] float GetUnscaledDeltaTime() const;

	[[nodiscard]] float GetFixedDeltaTime() const;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const;

	[[nodiscard]] float GetFrameCount() const;

private:
	const TimeSystem* timeSystem{ nullptr };
};
