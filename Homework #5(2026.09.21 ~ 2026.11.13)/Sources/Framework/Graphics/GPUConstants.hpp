#pragma once

#include "../Core/Common.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"
#include "../Math/Vector2D.hpp"
#include "../Math/Vector3D.hpp"
#include "../Math/Vector4D.hpp"

namespace TUK::Framework
{
	struct alignas(16) CameraConstants final
	{
		static constexpr u32 ROOT_SLOT = 0;

		Matrix4x4<f32> view{};
		Matrix4x4<f32> projection{};
		Vector3D<f32> lightDirection{ -0.5f, -1.0f, 0.5f };
		f32 lightIntensity = 1.0f;
		ColorRGBA<f32> lightColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> ambientLightColor{ 0.15f, 0.15f, 0.15f, 1.0f };
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

	struct alignas(16) DirectionalLightConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		Vector3D<f32> direction{ -0.5f, -1.0f, 0.5f };
		f32 intensity = 1.0f;
		ColorRGBA<f32> color{ 1.0f, 1.0f, 1.0f, 1.0f };
		ColorRGBA<f32> ambientColor{ 0.15f, 0.15f, 0.15f, 1.0f };
	};

	struct alignas(16) BillboardConstants final
	{
		static constexpr u32 ROOT_SLOT = 2;

		Vector2D<f32> size{ 1.0f, 1.0f };
		u32 frameIndex = 0;
		u32 columns = 1;
		u32 rows = 1;
		Vector3D<f32> padding{};
		ColorRGBA<f32> color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};
}
