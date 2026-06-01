#pragma once

#include "Component.h"

class StageSceneController final : public Component
{
public:
	StageSceneController() = default;
	~StageSceneController() override = default;

protected:
	void OnStart() override;
	void OnUpdate(float deltaTime_) override;

private:
	[[nodiscard]] int CountAliveEnemies() const;

	int initialEnemyCount{ 0 };
	bool clearTriggered{ false };
};

