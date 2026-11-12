#pragma once

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	struct alignas(16) CameraConstants final
	{
		static constexpr u32 ROOT_SLOT = 0;

		DirectX::XMFLOAT4X4 view{};
		DirectX::XMFLOAT4X4 projection{};
	};
	static_assert(sizeof(CameraConstants) == 128);

	struct alignas(16) ObjectConstants final
	{
		static constexpr u32 ROOT_SLOT = 1;

		DirectX::XMFLOAT4X4 world{};
	};
	static_assert(sizeof(ObjectConstants) == 64);

	struct alignas(16) MaterialConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		DirectX::XMFLOAT4 baseColor{};
		DirectX::XMFLOAT4 emissiveColor{};
		f32 metallic = 0.0f;
		f32 roughness = 0.0f;
		u32 textureMask = 0;
		f32 padding = 0.0f;
		DirectX::XMFLOAT4 specularColor{};
	};
	static_assert(sizeof(MaterialConstants) == 64);

	struct alignas(16) SkyboxConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
	static_assert(sizeof(SkyboxConstants) == 16);

	struct alignas(16) ImageUIConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		DirectX::XMFLOAT2 viewportSize{};
		DirectX::XMFLOAT2 padding{};
		DirectX::XMFLOAT4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
	static_assert(sizeof(ImageUIConstants) == 32);

	struct alignas(16) TextUIConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		DirectX::XMFLOAT2 viewportSize{};
		DirectX::XMFLOAT2 padding{};
		DirectX::XMFLOAT4 glyphRect{};
		DirectX::XMFLOAT4 glyphUV{};
		DirectX::XMFLOAT4 textColor{};
	};
	static_assert(sizeof(TextUIConstants) == 64);
}
