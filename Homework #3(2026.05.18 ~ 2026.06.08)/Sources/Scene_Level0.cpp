#include "Precompiled.h"
#include "Scene_Level0.h"

std::wstring_view Scene_Level0::GetTerrainPath() const noexcept
{
	return L"Resources/Terrains/HeightMap0.raw";
}

std::wstring_view Scene_Level0::GetTerrainMaterialPath() const noexcept
{
	return L"Resources/Materials/Terrain_Grassland.bin";
}

std::wstring_view Scene_Level0::GetTerrainObjectName() const noexcept
{
	return L"Terrain_Tutorial";
}

Vector3D Scene_Level0::GetPlayerSpawnPosition() const noexcept
{
	return Vector3D(0.0f, 200.0f, 0.0f);
}

Vector3D Scene_Level0::GetPlayerColliderSize() const noexcept
{
	return Vector3D(4.0f, 4.5f, 24.0f);
}

Vector3D Scene_Level0::GetPlayerColliderCenter() const noexcept
{
	return Vector3D(0.0f, 2.0f, -6.5f);
}

Vector3D Scene_Level0::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(240.0f, 0.0f, 240.0f);
}

int Scene_Level0::GetEnemySpawnCount() const noexcept
{
	return 0;
}

Vector3D Scene_Level0::GetLightDirection() const noexcept
{
	return Vector3D(-0.78f, -0.22f, 0.58f);
}
