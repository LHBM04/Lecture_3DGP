#include "Precompiled.h"
#include "Scene_Level3.h"

std::wstring_view Scene_Level3::GetTerrainPath() const noexcept
{
	return L"Resources/Terrains/HeightMap3.raw";
}

std::wstring_view Scene_Level3::GetTerrainMaterialPath() const noexcept
{
	return L"Resources/Materials/Terrain_Snowfield.bin";
}

std::wstring_view Scene_Level3::GetTerrainObjectName() const noexcept
{
	return L"Terrain_Snow";
}

Vector3D Scene_Level3::GetPlayerSpawnPosition() const noexcept
{
	return Vector3D(0.0f, 30.0f, 50.0f);
}

Vector3D Scene_Level3::GetPlayerColliderSize() const noexcept
{
	return Vector3D(8.0f, 4.0f, 12.0f);
}

Vector3D Scene_Level3::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(180.0f, 0.0f, 180.0f);
}

int Scene_Level3::GetEnemySpawnCount() const noexcept
{
	return 7;
}

Vector3D Scene_Level3::GetLightDirection() const noexcept
{
	return Vector3D(-0.2f, -1.0f, 0.4f);
}

ColorRGBA Scene_Level3::GetLightColor() const noexcept
{
	return ColorRGBA(0.92f, 0.96f, 1.0f, 1.0f);
}
