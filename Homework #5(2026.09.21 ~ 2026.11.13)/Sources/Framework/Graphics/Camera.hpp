#pragma once

#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Entities/Component.hpp"

#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	class Entity;

	enum class CameraClearFlags : u8
	{
		SolidColor,
		DepthOnly,
		Nothing
	};

	enum class CameraProjectionType : u8
	{
		Perspective,
		Orthographic
	};

	class Camera : public Component
	{
	public:
		explicit Camera(Entity& owner) noexcept;
		~Camera() noexcept override = default;

		void OnEnable() override;
		void OnDisable() override;

		[[nodiscard]] CameraClearFlags GetClearFlags() const noexcept;
		void SetClearFlags(CameraClearFlags value) noexcept;

		[[nodiscard]] CameraProjectionType GetProjectionType() const noexcept;
		void SetProjectionType(CameraProjectionType value) noexcept;

		[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;
		void SetViewport(const D3D12_VIEWPORT& value) noexcept;

		[[nodiscard]] const D3D12_RECT& GetScissorRect() const noexcept;
		void SetScissorRect(const D3D12_RECT& value) noexcept;

		[[nodiscard]] Matrix4x4<float> GetViewMatrix() const noexcept;
		[[nodiscard]] Matrix4x4<float> GetProjectionMatrix() const noexcept;

	private:
		CameraClearFlags clearFlags;
		CameraProjectionType projectionType;

		float fieldOfView;

		float nearPlane;
		float farPlane;

		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;

		float orthoSize;
	};

	inline Camera::Camera(Entity& owner) noexcept
		: Component(owner)
		, clearFlags(CameraClearFlags::SolidColor)
		, projectionType(CameraProjectionType::Perspective)
		, fieldOfView(60.0f)
		, nearPlane(0.1f)
		, farPlane(1000.0f)
		, viewport(0.0f, 0.0f, 1280.0f, 960.0f)
		, scissorRect(0, 0, 1280, 960)
		, orthoSize(10.0f)
	{
	}

	inline CameraClearFlags Camera::GetClearFlags() const noexcept
	{
		return clearFlags;
	}

	inline void Camera::SetClearFlags(CameraClearFlags value) noexcept
	{
		clearFlags = value;
	}

	inline CameraProjectionType Camera::GetProjectionType() const noexcept
	{
		return projectionType;
	}

	inline void Camera::SetProjectionType(CameraProjectionType value) noexcept
	{
		projectionType = value;
	}

	inline const D3D12_VIEWPORT& Camera::GetViewport() const noexcept
	{
		return viewport;
	}

	inline void Camera::SetViewport(const D3D12_VIEWPORT& value) noexcept
	{
		viewport = CD3DX12_VIEWPORT(value);
	}

	inline const D3D12_RECT& Camera::GetScissorRect() const noexcept
	{
		return scissorRect;
	}

	inline void Camera::SetScissorRect(const D3D12_RECT& value) noexcept
	{
		scissorRect = CD3DX12_RECT(value);
	}
}
