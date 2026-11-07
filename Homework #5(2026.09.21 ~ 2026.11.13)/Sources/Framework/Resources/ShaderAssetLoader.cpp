#include "Precompiled.hpp"
#include "ShaderAssetLoader.hpp"

#include <utility>

#include "../Graphics/GPUShader.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	std::expected<std::unique_ptr<ShaderAsset>, std::string> ShaderAssetLoader::Load(
		const std::filesystem::path& path) const
	{
		if (path.extension() != ".cso")
		{
			return std::unexpected("셰이더 로더에는 .cso 파일이 필요합니다.");
		}

		auto shader = GPUShader::Create(path);
		if (!shader)
		{
			return std::unexpected(shader.error());
		}

		return std::make_unique<ShaderAsset>(std::move(*shader), path);
	}
}
