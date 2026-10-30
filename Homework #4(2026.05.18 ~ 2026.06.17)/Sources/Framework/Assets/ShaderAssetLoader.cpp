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

		GPUShaderCreateInfo createInfo{};
		createInfo.path = path;
		createInfo.entryPoint = "main";
		createInfo.compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;

		const std::string stageExtension = path.stem().extension().string();
		if (stageExtension == ".vert")
		{
			createInfo.stage = ShaderStage::Vertex;
		}
		else if (stageExtension == ".tesc")
		{
			createInfo.stage = ShaderStage::Hull;
		}
		else if (stageExtension == ".tese")
		{
			createInfo.stage = ShaderStage::Domain;
		}
		else if (stageExtension == ".geom")
		{
			createInfo.stage = ShaderStage::Geometry;
		}
		else if (stageExtension == ".frag")
		{
			createInfo.stage = ShaderStage::Pixel;
		}
		else if (stageExtension == ".comp")
		{
			createInfo.stage = ShaderStage::Compute;
		}
		else
		{
			return std::unexpected("Unsupported shader stage extension: " + stageExtension);
		}

		auto shader = device.CreateGPUShader(createInfo);
		if (!shader)
		{
			return std::unexpected(shader.error());
		}

		return std::make_unique<ShaderAsset>(std::move(*shader), std::move(createInfo));
	}
}
