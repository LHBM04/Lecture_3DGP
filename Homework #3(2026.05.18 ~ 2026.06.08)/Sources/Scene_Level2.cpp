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
	return Vector3D(0.0f, 200.0f, 50.0f);
}

Vector3D Scene_Level2::GetPlayerColliderSize() const noexcept
{
	return Vector3D(30.0f, 18.0f, 82.0f);
}

Vector3D Scene_Level2::GetPlayerColliderCenter() const noexcept
{
	return Vector3D(2.0f, 1.4f, -28.0f);
}

Vector3D Scene_Level2::GetEnemySpawnHalfExtents() const noexcept
{
	return Vector3D(320.0f, 0.0f, 320.0f);
}

int Scene_Level2::GetEnemySpawnCount() const noexcept
{
	return 5;
}

Vector3D Scene_Level2::GetLightDirection() const noexcept
{
	return Vector3D(0.2f, -1.0f, 0.6f);
}

ColorRGBA Scene_Level2::GetSkyColor() const noexcept
{
	return ColorRGBA(0.85f, 0.78f, 0.65f, 1.0f); // Dusty Desert Yellow
}
