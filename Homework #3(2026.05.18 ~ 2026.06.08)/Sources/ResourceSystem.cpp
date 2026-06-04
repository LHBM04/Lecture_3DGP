#include "Precompiled.h"
#include "ResourceSystem.h"

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

void ResourceSystem::Initialize()
{
	if (!resources.empty())
	{
		return;
	}

	if (!std::filesystem::exists("Resources"))
	{
		return;
	}

	std::vector<std::filesystem::path> shaderPaths;
	std::vector<std::filesystem::path> meshPaths;
	std::vector<std::filesystem::path> materialPaths;

	for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator("Resources"))
	{
		if (!entry.is_regular_file())
		{
			continue;
		}

		const std::filesystem::path path{ entry.path().lexically_normal() };
		const std::wstring extension{ path.extension().wstring() };
		const std::wstring parentFolder{ path.parent_path().filename().wstring() };

		if (extension == L".hlsl" && parentFolder == L"Shaders")
		{
			shaderPaths.push_back(path);
		}
		else if (extension == L".bin" && parentFolder == L"Meshes")
		{
			meshPaths.push_back(path);
		}
		else if (extension == L".bin" && parentFolder == L"Materials")
		{
			materialPaths.push_back(path);
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

	std::unique_ptr<Material> defaultMaterial{ std::make_unique<Material>() };
	defaultMaterial->SetName(L"DefaultMaterial");
	defaultMaterial->SetColor(ColorRGBA::GetWhite());
	defaultMaterial->SetShader(GetResource<Shader>(L"Resources/Shaders/GameObject.hlsl"));
	const std::array<Material::InputElement, 2> defaultInputLayout
	{
		Material::InputElement{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 },
		Material::InputElement{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 }
	};
	defaultMaterial->SetInputLayout(defaultInputLayout);
	defaultMaterial->Load();

	resources.emplace(L"DefaultMaterial", std::move(defaultMaterial));
}

void ResourceSystem::Release()
{
	for (auto& [key, resource] : resources)
	{
		if (resource != nullptr)
		{
			resource->Unload();
		}
	}

	resources.clear();
}

void ResourceSystem::UnloadResource(const std::filesystem::path& path_)
{
	const std::wstring key{ path_.lexically_normal().wstring() };
	if (const auto iter{ resources.find(key) }; iter != resources.end())
	{
		if (iter->second != nullptr)
		{
			iter->second->Unload();
		}

		resources.erase(iter);
	}
}
