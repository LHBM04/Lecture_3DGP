#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Quaternion.h"
#include "Resource.h"
#include "Vector3D.h"

class GameObject;
class Material;
class Mesh;
class Scene;
class Transform;

struct ModelRendererData final
{
	std::wstring meshName;
	std::wstring materialName;

	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};

struct ModelNodeData final
{
	std::wstring name;
	int32_t parentIndex{ -1 };

	Vector3D localPosition{ Vector3D::GetZero() };
	Quaternion localRotation{ Quaternion::GetIdentity() };
	Vector3D localScale{ Vector3D::GetOne() };

	std::vector<ModelRendererData> renderers;
};

class Model final : public Resource
{
public:
	Model() = default;
	~Model() override = default;

	bool Load() override;
	void Unload() override;

	[[nodiscard]] const std::wstring& GetModelName() const noexcept;
	[[nodiscard]] const std::vector<ModelNodeData>& GetNodes() const noexcept;

private:
	[[nodiscard]] std::wstring BuildMeshPath(std::wstring_view meshName_) const;
	[[nodiscard]] std::wstring BuildMaterialPath(std::wstring_view materialName_) const;

private:
	std::wstring modelName;
	std::vector<ModelNodeData> nodes;
};
