#include "Precompiled.h"
#include "ResourceSystem.h"

#include "Logger.h"
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
	Logger::Trace(L"[Resource] Initialize begin");

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

		uint32_t shaderLoaded{ 0 };
		uint32_t meshLoaded{ 0 };
		uint32_t materialLoaded{ 0 };
		uint32_t shaderFailed{ 0 };
		uint32_t meshFailed{ 0 };
		uint32_t materialFailed{ 0 };

		Logger::Trace(L"[Resource] Discovered shaders={} meshes={} materials={}",
			static_cast<uint32_t>(shaderPaths.size()),
			static_cast<uint32_t>(meshPaths.size()),
			static_cast<uint32_t>(materialPaths.size()));

		for (const std::filesystem::path& path : shaderPaths)
		{
			if (LoadResource<Shader>(path) != nullptr)
			{
				++shaderLoaded;
				Logger::Trace(L"[Resource] Shader loaded '{}'", path.wstring());
			}
			else
			{
				++shaderFailed;
				Logger::Error(L"[Resource] Shader load failed '{}'", path.wstring());
			}
		}
		for (const std::filesystem::path& path : meshPaths)
		{
			if (LoadResource<Mesh>(path) != nullptr)
			{
				++meshLoaded;
			}
			else
			{
				++meshFailed;
				Logger::Error(L"[Resource] Mesh load failed '{}'", path.wstring());
			}
		}
		for (const std::filesystem::path& path : materialPaths)
		{
			if (LoadResource<Material>(path) != nullptr)
			{
				++materialLoaded;
			}
			else
			{
				++materialFailed;
				Logger::Error(L"[Resource] Material load failed '{}'", path.wstring());
			}
		}

		Logger::Info(L"[Resource] Loaded shaders={}/{} meshes={}/{} materials={}/{}",
			shaderLoaded, static_cast<uint32_t>(shaderPaths.size()),
			meshLoaded, static_cast<uint32_t>(meshPaths.size()),
			materialLoaded, static_cast<uint32_t>(materialPaths.size()));
		if (shaderFailed > 0 || meshFailed > 0 || materialFailed > 0)
		{
			Logger::Warning(L"[Resource] Failures shaders={} meshes={} materials={}",
				shaderFailed, meshFailed, materialFailed);
		}
	}
	else
	{
		Logger::Error(L"[Resource] Resources folder not found");
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
		else
		{
			Logger::Warning(L"[Resource] Default shader not found for DefaultMaterial");
		}
		
		resources[L"DefaultMaterial"] = std::move(defaultMat);
		Logger::Trace(L"[Resource] DefaultMaterial created");
	}
	Logger::Info(L"[Resource] Initialize end. totalResources={}", static_cast<uint32_t>(resources.size()));
}

void ResourceSystem::Release()
{
	Logger::Trace(L"[Resource] Release begin. totalResources={}", static_cast<uint32_t>(resources.size()));
	for (std::pair<const std::wstring, std::unique_ptr<Resource>>& pair : resources)
	{
		if (pair.second != nullptr)
		{
			pair.second->Unload();
		}
	}
	resources.clear();
	Logger::Info(L"[Resource] Release end");
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
