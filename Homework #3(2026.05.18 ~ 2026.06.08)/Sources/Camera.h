#pragma once

#include <DirectXCollision.h>

#include "ColorRGBA.h"
#include "Component.h"
#include "Matrix4x4.h"
#include "Vector4D.h"

class CubeCollider;
class Matrix4x4;
class SphereCollider;
class Vector2D;
class Vector3D;

struct alignas(256) CameraConstants final
{
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewProjectionMatrix;
};

class Camera final : public Component
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

	[[nodiscard]] float GetNearClipPlane() const;
	void SetNearClipPlane(float nearClipPlane_);

	[[nodiscard]] float GetFarClipPlane() const;
	void SetFarClipPlane(float farClipPlane_);

	[[nodiscard]] const Vector4D& GetViewport() const;
	void SetViewport(const Vector4D& viewport_);

	[[nodiscard]] ProjectionType GetProjectionType() const;
	void SetProjectionType(ProjectionType projectionType_);

	[[nodiscard]] float GetFOV() const;
	void SetFOV(float fov_);

	[[nodiscard]] float GetOrthographicSize() const;
	void SetOrthographicSize(float orthographicSize_);

	[[nodiscard]] ClearType GetClearMode() const;
	void SetClearMode(ClearType clearMode_);

	[[nodiscard]] const ColorRGBA& GetClearColor() const;
	void SetClearColor(const ColorRGBA& clearColor_);

	[[nodiscard]] Matrix4x4 GetViewMatrix() const;
	[[nodiscard]] Matrix4x4 GetProjectionMatrix() const;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix() const;

	[[nodiscard]] const DirectX::BoundingFrustum& GetFrustum() const;
	void UpdateFrustum();

	[[nodiscard]] bool IsInFrustum(const SphereCollider* collider_) const;
	[[nodiscard]] bool IsInFrustum(const CubeCollider* collider_) const;

	void ScreenPointToRay(const Vector2D& screenPoint_, Vector3D& rayOrigin_, Vector3D& rayDir_) const;

protected:
	void OnEnable() override;
	void OnDisable() override;

private:
	[[nodiscard]] Vector4D GetViewportPixelRect() const;

	float nearPlane{ 0.1f };
	float farPlane{ 100.0f };
	Vector4D viewportRect{ 0.0f, 0.0f, 1.0f, 1.0f };

	ProjectionType projectionType{ ProjectionType::Perspective };
	
	float fov{ 60.0f };
	float orthographicSize{ 5.0f };
	
	ClearType clearType{ ClearType::SolidColor };
	ColorRGBA clearColor{ ColorRGBA::GetBlue() };

	DirectX::BoundingFrustum frustum;
};
