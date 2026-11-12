#pragma once

#include "../Core/Common.hpp"

#include "AssetImporter.hpp"
#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ShaderAssetImporter final : public AssetImporter<ShaderAsset>
	{
	public:
		using AssetImporter<ShaderAsset>::AssetImporter;

		[[nodiscard]] std::expected<std::unique_ptr<ShaderAsset>, std::string> Import(
			const std::filesystem::path& path) const override;
	};
}
