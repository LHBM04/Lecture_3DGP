#pragma once

struct TimeContext final
{
	float timeScale{ 1.0f };
	float deltaTime{ 0.0f };
	float fixedDeltaTime{ 0.0f };
	float unscaledDeltaTime{ 0.0f };
	float unscaledFixedDeltaTime{ 0.0f };
	float totalTime{ 0.0f };
	float unscaledTotalTime{ 0.0f };
	int fps{ 0 };
};
