#pragma once

#include "../Graphics/GPUTexture2D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class TextureAsset final : public Asset
	{
	public:
		explicit TextureAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept;

		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;

	private:
		GPUTexture2D texture;
	};

	inline TextureAsset::TextureAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept
		: Asset(std::move(path))
		, texture(std::move(texture))
	{
	}

	inline const GPUTexture2D& TextureAsset::GetTexture() const noexcept
	{
		return texture;
	}
}
