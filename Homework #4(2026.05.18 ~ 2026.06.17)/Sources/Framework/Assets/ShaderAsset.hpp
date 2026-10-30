#pragma once

#include <utility>

#include "Asset.hpp"
#include "Framework/Graphics/D3DShader.hpp"

namespace TUK::Framework
{
	class ShaderAsset : public Asset
	{
	public:
		ShaderAsset(D3DShader shader) noexcept;
		~ShaderAsset() noexcept override = default;

		ShaderAsset(const ShaderAsset&) = delete;
		ShaderAsset& operator=(const ShaderAsset&) = delete;

		ShaderAsset(ShaderAsset&&) noexcept = default;
		ShaderAsset& operator=(ShaderAsset&&) noexcept = default;

		[[nodiscard]] const D3DShader& GetShader() const noexcept;

	private:
		D3DShader shader;
	};

	inline const D3DShader& ShaderAsset::GetShader() const noexcept
	{
		return shader;
	}

	inline ShaderAsset::ShaderAsset(D3DShader shader) noexcept
		: shader(std::move(shader))
	{
	}
}
