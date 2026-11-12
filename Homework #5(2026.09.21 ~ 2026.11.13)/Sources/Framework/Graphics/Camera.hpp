#pragma once

#include <expected>
#include <optional>
#include <string>

#include <directx/d3dx12.h>

#include "../Core/Common.hpp"

#include "../Objects/Component.hpp"

#include "../Math/ColorRGBA.hpp"
#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	enum class CameraProjectionType : u8
	{
		Perspective,
		Orthographic
	};

	class Camera : public Component
	{
	public:
		using Component::Component;

		void OnEnable() override;
		void OnDisable() override;

		[[nodiscard]] const ColorRGBA<f32>& GetClearColor() const noexcept;
		void SetClearColor(const ColorRGBA<f32>& value) noexcept;

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
		ColorRGBA<f32> clearColor = ColorRGBA<f32>::GetBlack();
		CameraProjectionType projectionType = CameraProjectionType::Perspective;

		f32 fieldOfView = 60.0f;

		f32 nearPlane = 0.1f;
		f32 farPlane = 1000.0f;

		CD3DX12_VIEWPORT viewport{ 0.0f, 0.0f, 800.0f, 600.0f };
		CD3DX12_RECT scissorRect{ 0, 0, 800, 600 };

		f32 orthoSize = 10.0f;
	};

	inline const ColorRGBA<f32>& Camera::GetClearColor() const noexcept
	{
		return clearColor;
	}

	inline void Camera::SetClearColor(const ColorRGBA<f32>& value) noexcept
	{
		clearColor = value;
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
