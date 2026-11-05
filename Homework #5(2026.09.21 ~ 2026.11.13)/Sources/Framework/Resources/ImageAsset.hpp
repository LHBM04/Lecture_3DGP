#pragma once

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "../Graphics/GPUTexture2D.hpp"

#include "Asset.hpp"

namespace TUK::Framework
{
	class ImageAsset final : public Asset
	{
	public:
		using Asset::Asset;

		ImageAsset() noexcept = default;

		[[nodiscard]] std::string Serialize() const override;
		[[nodiscard]] bool Deserialize(std::string_view text) override;

		[[nodiscard]] bool HasTexture() const noexcept;
		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;
		void SetTexture(GPUTexture2D value) noexcept;

	private:
		std::optional<GPUTexture2D> texture;
	};

	inline bool ImageAsset::HasTexture() const noexcept
	{
		return texture.has_value();
	}

	inline const GPUTexture2D& ImageAsset::GetTexture() const noexcept
	{
		assert(texture);
		return *texture;
	}

	inline void ImageAsset::SetTexture(GPUTexture2D value) noexcept
	{
		texture = std::move(value);
	}
}
