#include "Precompiled.hpp"
#include "ShaderAssetLoader.hpp"

#include <utility>

#include "../Graphics/GPUShader.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<Asset>, std::string> ShaderAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".hlsl")
		{
			return std::unexpected("ShaderAssetLoader requires an .hlsl file.");
		}

		LPCTSTR target = nullptr;

		const std::string stageExtension = path.stem().extension().string();
		if (stageExtension == ".vert")
		{
			target = TEXT("vs_6_0");
		}
		else if (stageExtension == ".tesc")
		{
			target = TEXT("hs_6_0");
		}
		else if (stageExtension == ".tese")
		{
			target = TEXT("ds_6_0");
		}
		else if (stageExtension == ".geom")
		{
			target = TEXT("gs_6_0");
		}
		else if (stageExtension == ".frag")
		{
			target = TEXT("ps_6_0");
		}
		else if (stageExtension == ".comp")
		{
			target = TEXT("cs_6_0");
		}
		else
		{
			return std::unexpected("Unsupported shader stage extension: " + stageExtension);
		}

		auto shader = GPUShader::Create(path, target, TEXT("main"));
		if (!shader)
		{
			return std::unexpected(shader.error());
		}

		return std::make_unique<ShaderAsset>(std::move(*shader), path);
	}
}
