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
	return Vector3D(0.0f, 200.0f, 0.0f);
}

Vector3D Scene_Level1::GetPlayerColliderSize() const noexcept
{
	return Vector3D(30.0f, 18.0f, 82.0f);
}

Vector3D Scene_Level1::GetPlayerColliderCenter() const noexcept
{
	return Vector3D(2.0f, 1.4f, -28.0f);
}

Vector3D Scene_Level1::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(300.0f, 0.0f, 300.0f);
}

int Scene_Level1::GetEnemySpawnCount() const noexcept
{
	return 5;
}

Vector3D Scene_Level1::GetLightDirection() const noexcept
{
	return Vector3D(0.0f, -1.0f, 1.0f);
}

ColorRGBA Scene_Level1::GetSkyColor() const noexcept
{
	return ColorRGBA(0.53f, 0.81f, 0.92f, 1.0f); // Bright Sky Blue
}
