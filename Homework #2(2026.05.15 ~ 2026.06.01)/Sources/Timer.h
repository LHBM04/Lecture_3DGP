#pragma once

namespace Timer
{
	void Initialize();
	void Tick();

	float GetTotalTime() noexcept;
	
	float GetDeltaTime() noexcept;
	float GetUnscaledDeltaTime() noexcept;
	
	float GetFixedDeltaTime() noexcept;
	float GetUnscaledFixedDeltaTime() noexcept;
	
	float GetTimeScale() noexcept;
	void SetTimeScale(float timeScale_) noexcept;
}
