#pragma once

#include "../Entities/Component.hpp"

#include "../Graphics/Rect.hpp"
#include "../Graphics/Viewport.hpp"

#include "../Math/Matrix4x4.hpp"

namespace TUK::Framework
{
	class Entity;

	enum class CameraClearFlags
	{
		SolidColor,
		DepthOnly,
		Nothing
	};

	enum class CameraProjectionType
	{
		Perspective,
		Orthographic
	};

	class Camera : public Component
	{
	public:
		Camera(Entity& owner) noexcept;
		~Camera() noexcept override;

		[[nodiscard]] CameraClearFlags GetClearFlags() const noexcept;
		void SetClearFlags(CameraClearFlags value) noexcept;

		[[nodiscard]] CameraProjectionType GetProjectionType() const noexcept;
		void SetProjectionType(CameraProjectionType value) noexcept;

		[[nodiscard]] const Viewport& GetViewport() const noexcept;
		void SetViewport(const Viewport& value) noexcept;

		[[nodiscard]] const Rect& GetScissorRect() const noexcept;
		void SetScissorRect(const Rect& value) noexcept;

		[[nodiscard]] Matrix4x4<float> GetViewMatrix() const noexcept;
		[[nodiscard]] Matrix4x4<float> GetProjectionMatrix() const noexcept;
		[[nodiscard]] Matrix4x4<float> GetViewProjectionMatrix() const noexcept;

	protected:
		void OnEnable() override;
		void OnDisable() override;

	private:
		CameraClearFlags clearFlags;
		CameraProjectionType projectionType;

		float fieldOfView;

		float nearPlane;
		float farPlane;

		Viewport viewport;
		Rect scissorRect;

		float orthoSize;
	};

	inline Camera::Camera(Entity& owner) noexcept
		: Component(owner)
		, clearFlags(CameraClearFlags::SolidColor)
		, projectionType(CameraProjectionType::Perspective)
		, fieldOfView(60.0f)
		, nearPlane(0.1f)
		, farPlane(100.0f)
		, viewport{ 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f }
		, scissorRect{ 0, 0, 800, 600 }
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

	inline const Viewport& Camera::GetViewport() const noexcept
	{
		return viewport;
	}

	inline void Camera::SetViewport(const Viewport& value) noexcept
	{
		viewport = value;
	}

	inline const Rect& Camera::GetScissorRect() const noexcept
	{
		return scissorRect;
	}

	inline void Camera::SetScissorRect(const Rect& value) noexcept
	{
		scissorRect = value;
	}
}
