#pragma once

#include <expected>
#include <string>

#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Entities/Component.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	class Cubemap;
	class Entity;
	class MaterialAsset;

	enum class CameraClearFlags : u8
	{
		SolidColor,
		DepthOnly,
		Nothing,
		Skybox
	};

	enum class CameraProjectionType : u8
	{
		Perspective,
		Orthographic
	};

	class Camera : public Component
	{
	public:
		struct BackgroundPass final
		{
			const MaterialAsset* material = nullptr;
			const Cubemap* cubemap = nullptr;
		};

		using Component::Component;

		void OnEnable() override;
		void OnDisable() override;

		[[nodiscard]] std::expected<void, std::string> PrepareRender(GPUCommandBuffer& commandBuffer) const;
		[[nodiscard]] BackgroundPass PrepareBackground(
			GPUCommandBuffer& commandBuffer, const MaterialAsset* skyboxMaterial) const;
		void RenderBackground(GPUCommandBuffer& commandBuffer, const BackgroundPass& pass) const;

		[[nodiscard]] CameraClearFlags GetClearFlags() const noexcept;
		void SetClearFlags(CameraClearFlags value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetBackgroundColor() const noexcept;
		void SetBackgroundColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] const Cubemap* GetCubemap() const noexcept;
		void SetCubemap(const Cubemap* value) noexcept;

		[[nodiscard]] CameraProjectionType GetProjectionType() const noexcept;
		void SetProjectionType(CameraProjectionType value) noexcept;

		[[nodiscard]] f32 GetFieldOfView() const noexcept;
		void SetFieldOfView(f32 value) noexcept;

		[[nodiscard]] f32 GetNearPlane() const noexcept;
		void SetNearPlane(f32 value) noexcept;

		[[nodiscard]] f32 GetFarPlane() const noexcept;
		void SetFarPlane(f32 value) noexcept;

		[[nodiscard]] f32 GetOrthoSize() const noexcept;
		void SetOrthoSize(f32 value) noexcept;

		[[nodiscard]] const CD3DX12_VIEWPORT& GetViewport() const noexcept;
		void SetViewport(const CD3DX12_VIEWPORT& value) noexcept;

		[[nodiscard]] const CD3DX12_RECT& GetScissorRect() const noexcept;
		void SetScissorRect(const CD3DX12_RECT& value) noexcept;

		[[nodiscard]] Matrix4x4<f32> GetViewMatrix() const noexcept;
		[[nodiscard]] Matrix4x4<f32> GetProjectionMatrix() const noexcept;

	private:
		bool ClearBuffers(GPUCommandBuffer& commandBuffer, bool clearColor) const;

		CameraClearFlags clearFlags = CameraClearFlags::SolidColor;
		ColorRGBA<f32> backgroundColor = ColorRGBA<f32>::GetBlack();
		const Cubemap* cubemap = nullptr;
		CameraProjectionType projectionType = CameraProjectionType::Perspective;

		f32 fieldOfView = 60.0f;

		f32 nearPlane = 0.1f;
		f32 farPlane = 1000.0f;

		CD3DX12_VIEWPORT viewport{ 0.0f, 0.0f, 800.0f, 600.0f };
		CD3DX12_RECT scissorRect{ 0, 0, 800, 600 };

		f32 orthoSize = 10.0f;
	};

	inline CameraClearFlags Camera::GetClearFlags() const noexcept
	{
		return clearFlags;
	}

	inline void Camera::SetClearFlags(CameraClearFlags value) noexcept
	{
		clearFlags = value;
	}

	inline const ColorRGBA<f32>& Camera::GetBackgroundColor() const noexcept
	{
		return backgroundColor;
	}

	inline void Camera::SetBackgroundColor(const ColorRGBA<f32>& value) noexcept
	{
		backgroundColor = value;
	}

	inline const Cubemap* Camera::GetCubemap() const noexcept
	{
		return cubemap;
	}

	inline void Camera::SetCubemap(const Cubemap* value) noexcept
	{
		cubemap = value;
	}

	inline CameraProjectionType Camera::GetProjectionType() const noexcept
	{
		return projectionType;
	}

	inline void Camera::SetProjectionType(CameraProjectionType value) noexcept
	{
		projectionType = value;
	}

	inline f32 Camera::GetFieldOfView() const noexcept
	{
		return fieldOfView;
	}

	inline void Camera::SetFieldOfView(f32 value) noexcept
	{
		fieldOfView = value;
	}

	inline f32 Camera::GetNearPlane() const noexcept
	{
		return nearPlane;
	}

	inline void Camera::SetNearPlane(f32 value) noexcept
	{
		nearPlane = value;
	}

	inline f32 Camera::GetFarPlane() const noexcept
	{
		return farPlane;
	}

	inline void Camera::SetFarPlane(f32 value) noexcept
	{
		farPlane = value;
	}

	inline f32 Camera::GetOrthoSize() const noexcept
	{
		return orthoSize;
	}

	inline void Camera::SetOrthoSize(f32 value) noexcept
	{
		orthoSize = value;
	}

	inline const CD3DX12_VIEWPORT& Camera::GetViewport() const noexcept
	{
		return viewport;
	}

	inline void Camera::SetViewport(const CD3DX12_VIEWPORT& value) noexcept
	{
		viewport = CD3DX12_VIEWPORT(value);
	}

	inline const CD3DX12_RECT& Camera::GetScissorRect() const noexcept
	{
		return scissorRect;
	}

	inline void Camera::SetScissorRect(const CD3DX12_RECT& value) noexcept
	{
		scissorRect = CD3DX12_RECT(value);
	}
}
