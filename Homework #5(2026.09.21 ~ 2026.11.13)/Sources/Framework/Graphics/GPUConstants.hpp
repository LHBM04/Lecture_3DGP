#pragma once

#include "../Core/Common.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector4D.hpp"

namespace TUK::Framework
{
	struct alignas(16) CameraConstants final
	{
		static constexpr u32 ROOT_SLOT = 0;

		Matrix4x4<f32> view{};
		Matrix4x4<f32> projection{};
	};
	
	struct alignas(16) ObjectConstants final
	{
		static constexpr u32 ROOT_SLOT = 1;

		Matrix4x4<f32> world{};
	};

	struct alignas(16) MaterialConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		ColorRGBA<f32> baseColor{};
		ColorRGBA<f32> emissiveColor{};
		f32 metallic = 0.0f;
		f32 roughness = 0.0f;
		u32 textureMask = 0;
		f32 padding = 0.0f;
		ColorRGBA<f32> specularColor{};
	};
	
	struct alignas(16) SkyboxConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		ColorRGBA<f32> color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
	
	struct alignas(16) ImageUIConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		Vector2D<f32> viewportSize{};
		Vector2D<f32> padding{};
		ColorRGBA<f32> color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
	
	struct alignas(16) TextUIConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		Vector2D<f32> viewportSize{};
		Vector2D<f32> padding{};
		Vector4D<f32> glyphRect{};
		Vector4D<f32> glyphUV{};
		ColorRGBA<f32> textColor{};
	};

	struct alignas(16) TerrainConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

	};
}
