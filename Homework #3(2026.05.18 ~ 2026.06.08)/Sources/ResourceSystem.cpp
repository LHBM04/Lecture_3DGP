#include "Precompiled.h"

#include "ResourceSystem.h"

#include <algorithm>

#include "AnimationClip.h"
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader.h"
#include "Terrain.h"
#include "Vector4D.h"
#include "Logger.h"

void ResourceSystem::Initialize()
{
	Logger::Trace(L"초기화를 시작합니다. 현재 경로={}", std::filesystem::current_path().wstring());

	std::size_t materialCount{ 0 };
	std::size_t meshCount{ 0 };
	std::size_t modelCount{ 0 };
	std::size_t terrainCount{ 0 };
	std::size_t shaderCount{ 0 };
	std::size_t animationCount{ 0 };

	if (std::filesystem::exists("Resources"))
	{
		std::vector<std::filesystem::path> shaderPaths;
		std::vector<std::filesystem::path> materialPaths;
		std::vector<std::filesystem::path> meshPaths;
		std::vector<std::filesystem::path> modelPaths;
		std::vector<std::filesystem::path> terrainPaths;
		std::vector<std::filesystem::path> animationPaths;

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
						materialPaths.emplace_back(path);
					}
					else if (parentFolder == L"Meshes")
					{
						meshPaths.emplace_back(path);
					}
					else if (parentFolder == L"Models")
					{
						modelPaths.emplace_back(path);
					}
					else if (parentFolder == L"Animations")
					{
						animationPaths.emplace_back(path);
					}
				}
				else if (extension == L".raw")
				{
					if (parentFolder == L"Terrains")
					{
						terrainPaths.emplace_back(path);
					}
				}
				else if (extension == L".hlsl")
				{
					if (parentFolder == L"Shaders")
					{
						shaderPaths.emplace_back(path);
					}
				}
			}
		}

		std::ranges::sort(shaderPaths);
		std::ranges::sort(materialPaths);
		std::ranges::sort(meshPaths);
		std::ranges::sort(modelPaths);
		std::ranges::sort(terrainPaths);
		std::ranges::sort(animationPaths);

		for (const std::filesystem::path& path : shaderPaths)
		{
			if (LoadResource<Shader>(path) != nullptr)
			{
				++shaderCount;
			}
		}

		for (const std::filesystem::path& path : materialPaths)
		{
			if (LoadResource<Material>(path) != nullptr)
			{
				++materialCount;
			}
		}

		for (const std::filesystem::path& path : meshPaths)
		{
			if (LoadResource<Mesh>(path) != nullptr)
			{
				++meshCount;
			}
		}

		for (const std::filesystem::path& path : modelPaths)
		{
			if (LoadResource<Model>(path) != nullptr)
			{
				++modelCount;
			}
		}

		for (const std::filesystem::path& path : terrainPaths)
		{
			if (LoadResource<Terrain>(path) != nullptr)
			{
				++terrainCount;
			}
		}

		for (const std::filesystem::path& path : animationPaths)
		{
			if (LoadResource<AnimationClip>(path) != nullptr)
			{
				++animationCount;
			}
		}
	}
	else
	{
		Logger::Critical(L"Resources 폴더를 찾을 수 없습니다.");
	}

	Logger::Info(
		L"초기화 완료. 머터리얼={}, 메쉬={}, 모델={}, 지형={}, 애니메이션={}, 쉐이더={}, 총 리소스={}",
		materialCount, meshCount, modelCount, terrainCount, animationCount, shaderCount, resources.size());
}

void ResourceSystem::Release()
{
	Logger::Trace(L"리소스를 해제합니다. 개수={}", resources.size());
	for (std::pair<const std::wstring, std::unique_ptr<Resource>>& pair : resources)
	{
		if (pair.second != nullptr)
		{
			pair.second->Unload();
		}
	}
	resources.clear();
	Logger::Trace(L"리소스 해제가 완료되었습니다.");
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
