#pragma once

#include <random>
#include <string_view>

#include "ColorRGBA.h"
#include "Scene.h"
#include "Vector3D.h"

class GameObject;
class Transform;

class Scene_Stage : public Scene
{
public:
	Scene_Stage() noexcept = default;
	~Scene_Stage() noexcept override = default;

protected:
	void OnLoad() override;
	void OnUnload() override;

	virtual std::wstring_view GetPlayerModelPath() const noexcept;
	virtual std::wstring_view GetEnemyModelPath() const noexcept;
	virtual std::wstring_view GetPlayerAnimationPath() const noexcept;
	virtual std::wstring_view GetTerrainPath() const noexcept = 0;
	virtual std::wstring_view GetTerrainMaterialPath() const noexcept = 0;
	virtual std::wstring_view GetTerrainObjectName() const noexcept = 0;

	virtual Vector3D GetPlayerSpawnPosition() const noexcept = 0;
	virtual Vector3D GetPlayerColliderSize() const noexcept = 0;
	virtual Vector3D GetPlayerColliderCenter() const noexcept;
	virtual Vector3D GetEnemySpawnerPosition() const noexcept;
	virtual Vector3D GetEnemySpawnHalfExtents() const noexcept = 0;
	virtual float GetEnemyMinDistanceFromPlayer() const noexcept;
	virtual int GetEnemySpawnCount() const noexcept = 0;
	virtual Vector3D GetLightDirection() const noexcept = 0;
	virtual ColorRGBA GetLightColor() const noexcept;
	virtual Vector3D GetCameraSpawnPosition() const noexcept;
	virtual Vector3D GetCameraOffset() const noexcept;

	void SpawnEnemy(const Vector3D& position_);

private:
	[[nodiscard]] GameObject* CreatePlayer();
	void CreateEnemies();
	void CreateLight();
	void CreateTerrain();
	void CreateCamera(Transform* playerTransform_);
	void PlayPlayerIdleAnimation(GameObject* playerObject_);

	[[nodiscard]] Vector3D GenerateSpawnPosition(const Vector3D& center_, const Vector3D* playerPosition_) noexcept;

	std::mt19937 randomEngine{ std::random_device{}() };
};
