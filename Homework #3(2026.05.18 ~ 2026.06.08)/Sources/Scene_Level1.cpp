#include "Precompiled.h"
#include "Scene_Level1.h"

std::wstring_view Scene_Level1::GetTerrainPath() const noexcept
{
	return L"Resources/Terrains/HeightMap1.raw";
}

std::wstring_view Scene_Level1::GetTerrainMaterialPath() const noexcept
{
	return L"Resources/Materials/Terrain_Grassland.bin";
}

std::wstring_view Scene_Level1::GetTerrainObjectName() const noexcept
{
	return L"Terrain_Grassland";
}

Vector3D Scene_Level1::GetPlayerSpawnPosition() const noexcept
{
	return Vector3D(0.0f, 30.0f, 0.0f);
}

Vector3D Scene_Level1::GetPlayerColliderSize() const noexcept
{
	return Vector3D(4.0f, 4.5f, 24.0f);
}

Vector3D Scene_Level1::GetPlayerColliderCenter() const noexcept
{
	return Vector3D(0.0f, 2.0f, -6.5f);
}

Vector3D Scene_Level1::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(150.0f, 0.0f, 150.0f);
}

int Scene_Level1::GetEnemySpawnCount() const noexcept
{
	return 5;
}

Vector3D Scene_Level1::GetLightDirection() const noexcept
{
	return Vector3D(-0.82f, -0.24f, 0.52f);
}

ColorRGBA Scene_Level1::GetSkyColor() const noexcept
{
	return ColorRGBA(0.45f, 0.78f, 1.0f, 1.0f);
}
