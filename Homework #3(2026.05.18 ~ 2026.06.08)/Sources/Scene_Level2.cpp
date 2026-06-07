#include "Precompiled.h"
#include "Scene_Level2.h"

std::wstring_view Scene_Level2::GetTerrainPath() const noexcept
{
	return L"Resources/Terrains/HeightMap2.raw";
}

std::wstring_view Scene_Level2::GetTerrainMaterialPath() const noexcept
{
	return L"Resources/Materials/Terrain_Desert.bin";
}

std::wstring_view Scene_Level2::GetTerrainObjectName() const noexcept
{
	return L"Terrain_Desert";
}

Vector3D Scene_Level2::GetPlayerSpawnPosition() const noexcept
{
	return Vector3D(0.0f, 30.0f, 50.0f);
}

Vector3D Scene_Level2::GetPlayerColliderSize() const noexcept
{
	return Vector3D(8.0f, 4.0f, 12.0f);
}

Vector3D Scene_Level2::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(160.0f, 0.0f, 160.0f);
}

int Scene_Level2::GetEnemySpawnCount() const noexcept
{
	return 5;
}

Vector3D Scene_Level2::GetLightDirection() const noexcept
{
	return Vector3D(0.2f, -1.0f, 0.6f);
}
