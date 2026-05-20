#pragma once

#include "Component.hpp"
#include <random>

class Mesh;

class ObstacleSpawner final : public Component
{
public:
	virtual ~ObstacleSpawner() override = default;

	[[nodiscard]] int GetMaxObstacleCount() const;
	void SetMaxObstacleCount(int maxObstacleCount_);

protected:
	void OnAttach() override;
	void OnUpdate() override;

private:
	void SpawnObstacles(int count_);
	[[nodiscard]] int GetCurrentObstacleCount() const;
	[[nodiscard]] int GetDesiredObstacleCount() const;
	Mesh* GetOrCreateObstacleMesh();

	int maxObstacleCount{ 50 };
	float minSpawnRadius{ 30.0f };
	float maxSpawnRadius{ 120.0f };
	float minObstacleScale{ 1.0f };
	float maxObstacleScale{ 5.0f };
	float minRotationSpeed{ 30.0f };
	float maxRotationSpeed{ 180.0f };

	int spawnSerial{ 0 };
	Mesh* obstacleMesh{ nullptr };
	std::mt19937 randomEngine{ std::random_device{}() };
};
