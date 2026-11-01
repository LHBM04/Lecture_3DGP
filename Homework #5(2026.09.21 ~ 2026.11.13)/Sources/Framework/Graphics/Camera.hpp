#pragma once

#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
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

	class Camera
	{
	public:
		[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;
		void SetViewport(const D3D12_VIEWPORT& value) noexcept;

		[[nodiscard]] const D3D12_RECT& GetScissorRect() const noexcept;
		void SetScissorRect(const D3D12_RECT& value) noexcept;

		[[nodiscard]] Matrix4x4<f32> GetViewMatrix() const noexcept;
		[[nodiscard]] Matrix4x4<f32> GetProjectionMatrix() const noexcept;

	private:
		float fieldOfView;

		float nearPlane;
		float farPlane;

		CD3DX12_VIEWPORT viewport;
		CD3DX12_RECT scissorRect;

		float orthoSize;
	};

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
