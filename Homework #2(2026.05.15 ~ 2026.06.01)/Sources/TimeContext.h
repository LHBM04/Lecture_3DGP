#pragma once

class TimeSystem;

class TimeContext
{
public:
	[[nodiscard]] float GetTotalTime() const;

	[[nodiscard]] float GetTimeScale() const;

	[[nodiscard]] float GetDeltaTime() const;
	[[nodiscard]] float GetUnscaledDeltaTime() const;

	[[nodiscard]] float GetFixedDeltaTime() const;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const;

	[[nodiscard]] float GetFrameCount() const;

private:
	TimeSystem& timeSystem;
};
