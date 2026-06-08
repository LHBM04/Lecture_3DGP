#pragma once

#include "Component.h"
#include "Vector3D.h"

class CubeCollider;
class Terrain;
class Transform;

class PlayerController final : public Component
{
public:
	PlayerController() = default;
	~PlayerController() override = default;

protected:
	void OnStart() override;
	void OnUpdate() override;
	void OnCollisionEnter(class Collider* other_) override;

private:
	[[nodiscard]] bool IsColliding(bool ignoreFloor_);
	void CacheTerrain();
	[[nodiscard]] bool IsCollidingWithTerrain() const;
	void TriggerGameOver(std::wstring_view causeName_, std::wstring_view causeTag_);

	Transform* headTransform{ nullptr };
	Terrain* terrain{ nullptr };
	Transform* terrainTransform{ nullptr };
	float moveSpeed{ 40.0f };
	float altitudeSpeed{ 30.0f };
	float rotationSpeed{ 180.0f };

	float fireTimer{ 0.0f };
	float fireCooldown{ 0.2f };
	float projectileSpeed{ 150.0f };
	float projectileLifetime{ 4.0f };
	float terrainCrashMargin{ 0.25f };
	bool gameOverTriggered{ false };
};
