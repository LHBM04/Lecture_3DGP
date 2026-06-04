#pragma once

#include "Singleton.h"

class TimeSystem final : public Singleton<TimeSystem>
{
public:
	TimeSystem() = default;
	~TimeSystem() override = default;

	void Reset();
	void Tick();

	[[nodiscard]] float GetElapseTime() const noexcept;

	[[nodiscard]] float GetTimeScale() const noexcept;
	void SetTimeScale(float timeScale_) noexcept;

	[[nodiscard]] float GetDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledDeltaTime() const noexcept;

	[[nodiscard]] float GetFixedDeltaTime() const noexcept;
	[[nodiscard]] float GetUnscaledFixedDeltaTime() const noexcept;

private:

};
