#pragma once

#include "System.h"

class TimeSystem : public ISystem
{
public:
	bool Initialize();
	void Release() override;

	void Reset();
	void Tick();

	float GetTotalTime() const;

	float GetTimeScale() const;

	float GetDeltaTime() const;
	float GetUnscaledDeltaTime() const;

	float GetFixedDeltaTime() const;
	float GetUnscaledFixedDeltaTime() const;

	float GetFrameCount() const;

private:
	LARGE_INTEGER frequency;

	LARGE_INTEGER lastTime; 
	LARGE_INTEGER startTime{};

	float deltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };

	float totalTime{ 0.0f };
	float unscaledTime{ 0.0f };

	float fixedDeltaTime{ 1.0f / 60.0f };
	float timeScale{ 1.0f };

	int frameCount{ 0 };
	int totalFrameCount{ 0 };

	float fpsTimer{ 0.0f };
	int currentFps{ 0 };
};
