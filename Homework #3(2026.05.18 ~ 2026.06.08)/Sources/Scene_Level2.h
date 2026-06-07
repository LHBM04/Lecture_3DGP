#pragma once

#include "Scene_Stage.h"

class Scene_Level2 final : public Scene_Stage
{
public:
	Scene_Level2() noexcept = default;
	~Scene_Level2() noexcept override = default;

protected:
	[[nodiscard]] std::wstring_view GetTerrainPath() const noexcept override;
	[[nodiscard]] std::wstring_view GetTerrainMaterialPath() const noexcept override;
	[[nodiscard]] std::wstring_view GetTerrainObjectName() const noexcept override;
	[[nodiscard]] Vector3D GetPlayerSpawnPosition() const noexcept override;
	[[nodiscard]] Vector3D GetPlayerColliderSize() const noexcept override;
	[[nodiscard]] Vector3D GetEnemySpawnHalfExtents() const noexcept override;
	[[nodiscard]] int GetEnemySpawnCount() const noexcept override;
	[[nodiscard]] Vector3D GetLightDirection() const noexcept override;
};
