#include "Precompiled.h"
#include "ResourceSystem.h"

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

std::wstring ResourceSystem::NormalizeKey(std::wstring key_)
{
	std::replace(key_.begin(), key_.end(), L'\\', L'/');
	return key_;
}

std::wstring ResourceSystem::NormalizeKey(std::wstring_view key_)
{
	return NormalizeKey(std::wstring{ key_ });
}

void ResourceSystem::Initialize()
{
	if (std::filesystem::exists("Resources"))
	{
		std::vector<std::filesystem::path> shaderPaths;
		std::vector<std::filesystem::path> meshPaths;
		std::vector<std::filesystem::path> materialPaths;

		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator("Resources"))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			const std::filesystem::path path{ entry.path() };
			const std::wstring extension{ path.extension().wstring() };
			const std::wstring parentFolder{ path.parent_path().filename().wstring() };

			if (extension == L".hlsl" && parentFolder == L"Shaders")
			{
				shaderPaths.emplace_back(path);
			}
			else if (extension == L".bin" && parentFolder == L"Meshes")
			{
				meshPaths.emplace_back(path);
			}
			else if (extension == L".bin" && parentFolder == L"Materials")
			{
				materialPaths.emplace_back(path);
			}
		}

		for (const std::filesystem::path& path : shaderPaths)
		{
			LoadResource<Shader>(path);
		}
		for (const std::filesystem::path& path : meshPaths)
		{
			LoadResource<Mesh>(path);
		}
		for (const std::filesystem::path& path : materialPaths)
		{
			LoadResource<Material>(path);
		}
	}

	// Create Default Resources
	{
		std::unique_ptr<Material> defaultMat{ std::make_unique<Material>() };
		defaultMat->SetName(L"DefaultMaterial");
		defaultMat->SetBaseColor(Vector4D{ 1.0f, 1.0f, 1.0f, 1.0f });
		
		Shader* defaultShader{ GetResource<Shader>(L"Resources/Shaders/GameObject.hlsl") };
		if (defaultShader != nullptr)
		{
			defaultMat->SetShader(defaultShader);
		}
		
		resources[L"DefaultMaterial"] = std::move(defaultMat);
	}
}

void ResourceSystem::Release()
{
	for (std::pair<const std::wstring, std::unique_ptr<Resource>>& pair : resources)
	{
		if (pair.second != nullptr)
		{
			pair.second->Unload();
		}
	}
	resources.clear();
}

void ResourceSystem::UnloadResource(const std::filesystem::path& path_)
{
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(NormalizeKey(path_.wstring())) }; it != resources.end())
	{
		if (it->second != nullptr)
		{
			it->second->Unload();
		}
		resources.erase(it);
	}
}
