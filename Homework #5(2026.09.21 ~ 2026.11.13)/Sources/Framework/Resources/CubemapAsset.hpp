#pragma once

#include <filesystem>
#include <utility>

#include "../Graphics/GPUTexture2D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class CubemapAsset final : public Asset
	{
	public:
		explicit CubemapAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept;

		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;

	private:
		GPUTexture2D texture;
	};

	inline CubemapAsset::CubemapAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept
		: Asset(std::move(path))
		, texture(std::move(texture))
	{
	}

	inline const GPUTexture2D& CubemapAsset::GetTexture() const noexcept
	{
		return texture;
	}
}
