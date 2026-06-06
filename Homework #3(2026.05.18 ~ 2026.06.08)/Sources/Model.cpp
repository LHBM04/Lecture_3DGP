#include "Precompiled.h"

#include "Model.h"

#include "GameObject.h"
#include "Logger.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "ResourceSystem.h"
#include "Scene.h"
#include "Transform.h"

namespace
{
	[[nodiscard]] bool HasDirectorySeparator(std::wstring_view value_) noexcept
	{
		return value_.find(L'/') != std::wstring_view::npos
			|| value_.find(L'\\') != std::wstring_view::npos;
	}

	[[nodiscard]] bool HasBinExtension(std::wstring_view value_) noexcept
	{
		const std::wstring_view extension{ L".bin" };
		return value_.size() >= extension.size()
			&& value_.substr(value_.size() - extension.size()) == extension;
	}
}

bool Model::Load()
{
	if (path.empty())
	{
		Logger::Critical(L"[Model] Load failed: path is empty.");
		return false;
	}

	std::ifstream file{ path, std::ios::binary };
	if (!file.is_open())
	{
		Logger::Critical(L"[Model] Load failed: cannot open file. path={}", path);
		return false;
	}

	if (!HasTag(file, "<Model>:"))
	{
		Logger::Critical(L"[Model] Load failed: <Model>: tag not found. path={}", path);
		return false;
	}

	modelName = ReadString(file);
	if (modelName.empty())
	{
		Logger::Critical(L"[Model] Load failed: model name is empty. path={}", path);
		return false;
	}

	if (!HasTag(file, "<Nodes>:"))
	{
		Logger::Critical(L"[Model] Load failed: <Nodes>: tag not found. path={}", path);
		return false;
	}

	uint32_t nodeCount{ 0 };
	if (!ReadValue(file, nodeCount))
	{
		Logger::Critical(L"[Model] Load failed: cannot read node count. path={}", path);
		return false;
	}

	nodes.clear();
	nodes.reserve(nodeCount);

	ResourceSystem& resourceSystem{ ResourceSystem::GetInstance() };

	for (uint32_t i{ 0 }; i < nodeCount; ++i)
	{
		if (!HasTag(file, "<Node>:"))
		{
			Logger::Critical(L"[Model] Load failed: <Node>: tag not found. path={}, index={}", path, i);
			return false;
		}

		int32_t nodeIndex{ -1 };
		ModelNodeData node{};
		if (!ReadValue(file, nodeIndex) || !ReadValue(file, node.parentIndex))
		{
			Logger::Critical(L"[Model] Load failed: cannot read node index. path={}, index={}", path, i);
			return false;
		}

		node.name = ReadString(file);
		if (node.name.empty())
		{
			Logger::Critical(L"[Model] Load failed: node name is empty. path={}, index={}", path, i);
			return false;
		}

		if (nodeIndex != static_cast<int32_t>(i))
		{
			Logger::Warning(L"[Model] Node index does not match read order. path={}, expected={}, actual={}", path, i, nodeIndex);
		}

		if (!HasTag(file, "<LocalPosition>:") || !ReadValue(file, node.localPosition))
		{
			Logger::Critical(L"[Model] Load failed: cannot read local position. path={}, node={}", path, node.name);
			return false;
		}

		if (!HasTag(file, "<LocalRotation>:") || !ReadValue(file, node.localRotation))
		{
			Logger::Critical(L"[Model] Load failed: cannot read local rotation. path={}, node={}", path, node.name);
			return false;
		}

		if (!HasTag(file, "<LocalScale>:") || !ReadValue(file, node.localScale))
		{
			Logger::Critical(L"[Model] Load failed: cannot read local scale. path={}, node={}", path, node.name);
			return false;
		}

		if (!HasTag(file, "<Renderers>:"))
		{
			Logger::Critical(L"[Model] Load failed: <Renderers>: tag not found. path={}, node={}", path, node.name);
			return false;
		}

		uint32_t rendererCount{ 0 };
		if (!ReadValue(file, rendererCount))
		{
			Logger::Critical(L"[Model] Load failed: cannot read renderer count. path={}, node={}", path, node.name);
			return false;
		}

		node.renderers.reserve(rendererCount);
		for (uint32_t rendererIndex{ 0 }; rendererIndex < rendererCount; ++rendererIndex)
		{
			if (!HasTag(file, "<Renderer>:"))
			{
				Logger::Critical(L"[Model] Load failed: <Renderer>: tag not found. path={}, node={}, renderer={}", path, node.name, rendererIndex);
				return false;
			}

			ModelRendererData renderer{};
			renderer.meshName = ReadString(file);
			renderer.materialName = ReadString(file);
			if (renderer.meshName.empty() || renderer.materialName.empty())
			{
				Logger::Critical(L"[Model] Load failed: renderer resource name is empty. path={}, node={}", path, node.name);
				return false;
			}

			const std::wstring meshPath{ BuildMeshPath(renderer.meshName) };
			const std::wstring materialPath{ BuildMaterialPath(renderer.materialName) };
			renderer.mesh = resourceSystem.GetResource<Mesh>(meshPath);
			renderer.material = resourceSystem.GetResource<Material>(materialPath);

			if (renderer.mesh == nullptr || renderer.material == nullptr)
			{
				Logger::Critical(
					L"[Model] Load failed: renderer resource not found. path={}, node={}, mesh={}, material={}",
					path,
					node.name,
					meshPath,
					materialPath);
				return false;
			}

			node.renderers.emplace_back(renderer);
		}

		nodes.emplace_back(std::move(node));
	}

	Logger::Trace(L"[Model] Loaded model. path={}, nodes={}", path, nodes.size());
	return true;
}

void Model::Unload()
{
	nodes.clear();
	modelName.clear();
}

const std::wstring& Model::GetModelName() const noexcept
{
	return modelName;
}

const std::vector<ModelNodeData>& Model::GetNodes() const noexcept
{
	return nodes;
}

std::wstring Model::BuildMeshPath(std::wstring_view meshName_) const
{
	std::wstring result{ meshName_ };
	if (!HasDirectorySeparator(result))
	{
		result = L"Resources/Meshes/" + result;
	}

	if (!HasBinExtension(result))
	{
		result += L".bin";
	}

	return result;
}

std::wstring Model::BuildMaterialPath(std::wstring_view materialName_) const
{
	std::wstring result{ materialName_ };
	if (!HasDirectorySeparator(result))
	{
		result = L"Resources/Materials/" + result;
	}

	if (!HasBinExtension(result))
	{
		result += L".bin";
	}

	return result;
}
