#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "GameObject.h"
#include "MathF.h"
#include "Matrix4x4.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector4D.h"

class Camera : public Component
{
public:
	enum class ProjectionType
	{
		Perspective,
		Orthographic
	};

	enum class ClearType
	{
		SolidColor,
		DepthOnly,
		Nothing
	};

	Camera() = default;
	~Camera() override = default;

	[[nodiscard]] float GetNearClipPlane() const noexcept;
	void SetNearClipPlane(float nearClipPlane_) noexcept;

	[[nodiscard]] float GetFarClipPlane() const noexcept;
	void SetFarClipPlane(float farClipPlane_) noexcept;

	[[nodiscard]] const Vector4D& GetViewport() const noexcept;
	void SetViewport(const Vector4D& viewport_) noexcept;

	[[nodiscard]] ProjectionType GetProjectionType() const noexcept;
	void SetProjectionType(ProjectionType projectionType_) noexcept;

	[[nodiscard]] float GetFOV() const noexcept;
	void SetFOV(float fov_) noexcept;

	[[nodiscard]] float GetOrthographicSize() const noexcept;
	void SetOrthographicSize(float orthographicSize_) noexcept;

	[[nodiscard]] ClearType GetClearMode() const noexcept;
	void SetClearMode(ClearType clearMode_) noexcept;

	[[nodiscard]] const ColorRGBA& GetClearColor() const noexcept;
	void SetClearColor(const ColorRGBA& clearColor_) noexcept;

	[[nodiscard]] Matrix4x4 GetViewMatrix() const;
	[[nodiscard]] Matrix4x4 GetProjectionMatrix() const;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix() const;

protected:
	void OnPreRender() override;
	void OnPostRender() override;

private:
	float nearPlane{ 0.1f };
	float farPlane{ 100.0f };
	Vector4D viewportRect{ 0.0f, 0.0f, 1.0f, 1.0f };

	ProjectionType projectionType{ ProjectionType::Perspective };
	
	float fov{ 60.0f };
	float orthographicSize{ 5.0f };
	
	ClearType clearType{ ClearType::SolidColor };
	ColorRGBA clearColor{ ColorRGBA::GetBlue() };
};

inline float Camera::GetNearClipPlane() const noexcept
{
	return nearPlane;
}

inline void Camera::SetNearClipPlane(float nearClipPlane_) noexcept
{
	nearPlane = nearClipPlane_;
}

inline float Camera::GetFarClipPlane() const noexcept
{
	return farPlane;
}

inline void Camera::SetFarClipPlane(float farClipPlane_) noexcept
{
	farPlane = farClipPlane_;
}

inline const Vector4D& Camera::GetViewport() const noexcept
{
	return viewportRect;
}

inline void Camera::SetViewport(const Vector4D& viewport_) noexcept
{
	viewportRect = viewport_;
}

inline Camera::ProjectionType Camera::GetProjectionType() const noexcept
{
	return projectionType;
}

inline void Camera::SetProjectionType(ProjectionType projectionType_) noexcept
{
	projectionType = projectionType_;
}

inline float Camera::GetFOV() const noexcept
{
	return fov;
}

inline void Camera::SetFOV(float fov_) noexcept
{
	fov = fov_;
}

inline float Camera::GetOrthographicSize() const noexcept
{
	return orthographicSize;
}

inline void Camera::SetOrthographicSize(float orthographicSize_) noexcept
{
	orthographicSize = orthographicSize_;
}

inline Camera::ClearType Camera::GetClearMode() const noexcept
{
	return clearType;
}

inline void Camera::SetClearMode(ClearType clearMode_) noexcept
{
	clearType = clearMode_;
}

inline const ColorRGBA& Camera::GetClearColor() const noexcept
{
	return clearColor;
}

inline void Camera::SetClearColor(const ColorRGBA& clearColor_) noexcept
{
	clearColor = clearColor_;
}

inline Matrix4x4 Camera::GetViewMatrix() const
{
	const Transform* transform{ GetOwner()->GetComponent<Transform>() };

	if (nullptr == transform)
	{
		return Matrix4x4::GetIdentity();
	}

	return transform->GetWorldMatrix().GetInverse();
}

inline Matrix4x4 Camera::GetProjectionMatrix() const
{
	const float aspectRatio{ viewportRect.z / viewportRect.w };

	switch (projectionType)
	{
	case ProjectionType::Perspective:
	{
		return Matrix4x4::Perspective(fov, aspectRatio, nearPlane, farPlane);
	}
	case ProjectionType::Orthographic:
	{
		const float halfHeight{ orthographicSize };
		const float halfWidth{ halfHeight * aspectRatio };

		return Matrix4x4::Ortho(
			-halfWidth, halfWidth, 
			-halfHeight, halfHeight, 
			GetNearClipPlane(), GetFarClipPlane());
	}
	}

	std::unreachable();
}

inline Matrix4x4 Camera::GetViewProjectionMatrix() const
{
	return GetViewMatrix() * GetProjectionMatrix();
}
