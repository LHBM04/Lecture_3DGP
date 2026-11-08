#pragma once

#include <filesystem>
#include <utility>

#include "../Graphics/GPUTexture2D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ImageAsset final : public Asset
	{
	public:
		explicit ImageAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept;

		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;

	private:
		GPUTexture2D texture;
	};

	inline ImageAsset::ImageAsset(std::filesystem::path path, GPUTexture2D&& texture) noexcept
		: Asset(std::move(path))
		, texture(std::move(texture))
	{
	}

	inline const GPUTexture2D& ImageAsset::GetTexture() const noexcept
	{
		return texture;
	}
}
