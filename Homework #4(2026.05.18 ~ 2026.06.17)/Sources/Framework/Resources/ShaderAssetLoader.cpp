#include "Precompiled.hpp"

#include <utility>

#include "ShaderAssetLoader.hpp"
#include "ShaderAsset.hpp"

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUShader.hpp"

namespace TUK::Framework
{
	ShaderAssetLoader::ShaderAssetLoader(const GPUDevice& device) noexcept
		: device(device)
	{
	}

	std::expected<std::unique_ptr<Asset>, std::string> ShaderAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".hlsl")
		{
			return std::unexpected("ShaderAssetLoader requires an .hlsl file.");
		}

		std::string_view target;

		const std::string stageExtension = path.stem().extension().string();
		if (stageExtension == ".vert")
		{
			target = "vs_5_1";
		}
		else if (stageExtension == ".tesc")
		{
			target = "hs_5_1";
		}
		else if (stageExtension == ".tese")
		{
			target = "ds_5_1";
		}
		else if (stageExtension == ".geom")
		{
			target = "gs_5_1";
		}
		else if (stageExtension == ".frag")
		{
			target = "ps_5_1";
		}
		else if (stageExtension == ".comp")
		{
			target = "cs_5_1";
		}
		else
		{
			return std::unexpected("Unsupported shader stage extension: " + stageExtension);
		}

		auto shader = device.CreateGPUShader(path, target, "main", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS);
		if (!shader)
		{
			return std::unexpected(shader.error());
		}

		return std::make_unique<ShaderAsset>(std::move(*shader));
	}
}
