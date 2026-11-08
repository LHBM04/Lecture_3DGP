#pragma once

#include "../Core/Common.hpp"

#include "AssetLoader.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ShaderAssetLoader final : public AssetLoader<ShaderAsset>
	{
	public:
		using AssetLoader<ShaderAsset>::AssetLoader;

		[[nodiscard]] std::expected<std::unique_ptr<ShaderAsset>, std::string> Load(
			const std::filesystem::path& path) const override;
	};
}
