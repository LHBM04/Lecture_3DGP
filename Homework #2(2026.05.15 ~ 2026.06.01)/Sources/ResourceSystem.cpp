#include "Precompiled.h"

#include "ResourceSystem.h"

#include <algorithm>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Vector4D.h"
#include "Logger.h"

void ResourceSystem::Initialize()
{
	Logger::Trace(L"[Resource] Initialize begin. cwd={}", std::filesystem::current_path().wstring());

	std::size_t materialCount{ 0 };
	std::size_t meshCount{ 0 };
	std::size_t shaderCount{ 0 };

	if (std::filesystem::exists("Resources"))
	{
		for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator("Resources"))
		{
			if (entry.is_regular_file())
			{
				const std::filesystem::path path{ entry.path() };
				const std::wstring extension{ path.extension().wstring() };
				const std::wstring parentFolder{ path.parent_path().filename().wstring() };

				if (extension == L".bin")
				{
					if (parentFolder == L"Materials")
					{
						if (LoadResource<Material>(path) != nullptr)
						{
							++materialCount;
						}
					}
					else if (parentFolder == L"Meshes")
					{
						if (LoadResource<Mesh>(path) != nullptr)
						{
							++meshCount;
						}
					}
				}
				else if (extension == L".hlsl")
				{
					if (parentFolder == L"Shaders")
					{
						if (LoadResource<Shader>(path) != nullptr)
						{
							++shaderCount;
						}
					}
				}
			}
		}
	}
	else
	{
		Logger::Critical(L"[Resource] Resources folder not found.");
	}

	// Create Default Resources
	{
		std::unique_ptr<Material> defaultMat{ std::make_unique<Material>() };
		defaultMat->SetName(L"DefaultMaterial");
		defaultMat->SetColor(ColorRGBA{ 1.0f, 1.0f, 1.0f, 1.0f });
		
		Shader* const defaultShader{ GetResource<Shader>(L"Resources/Shaders/GameObject.hlsl") };
		if (defaultShader != nullptr)
		{
			defaultMat->SetShader(defaultShader);
		}
		
		const std::wstring key{ NormalizeKey(L"DefaultMaterial") };
		resources[key] = std::move(defaultMat);
		Logger::Trace(L"[Resource] Created default material: {}", key);
	}

	Logger::Info(
		L"[Resource] Initialize done. materials={}, meshes={}, shaders={}, total={}",
		materialCount, meshCount, shaderCount, resources.size());
}

void ResourceSystem::Release()
{
	Logger::Trace(L"[Resource] Release begin. count={}", resources.size());
	for (std::pair<const std::wstring, std::unique_ptr<Resource>>& pair : resources)
	{
		if (pair.second != nullptr)
		{
			pair.second->Unload();
		}
	}
	resources.clear();
	Logger::Trace(L"[Resource] Release done.");
}

void ResourceSystem::UnloadResource(const std::filesystem::path& path_)
{
	const std::wstring key{ NormalizeKey(path_.wstring()) };
	if (std::unordered_map<std::wstring, std::unique_ptr<Resource>>::iterator it{ resources.find(key) }; it != resources.end())
	{
		if (it->second != nullptr)
		{
			it->second->Unload();
		}
		resources.erase(it);
	}
}

std::wstring ResourceSystem::NormalizeKey(std::wstring_view path_)
{
	std::wstring key{ path_ };
	std::replace(key.begin(), key.end(), L'\\', L'/');
	return key;
}
