#pragma once

#include "Component.h"
#include "CameraProjection.h"
#include "Matrix4x4.h"
#include "Vector4D.h"

class Camera : public Component
{
public:
	Camera();

	[[nodiscard]] CameraProjection* GetProjection() noexcept;
	[[nodiscard]] const CameraProjection* GetProjection() const noexcept;
	void SetProjection(std::unique_ptr<CameraProjection> projection_) noexcept;

	[[nodiscard]] float GetNearClipPlane() const noexcept;
	void SetNearClipPlane(float nearClipPlane_) noexcept;

	[[nodiscard]] float GetFarClipPlane() const noexcept;
	void SetFarClipPlane(float farClipPlane_) noexcept;

	[[nodiscard]] const Vector4D& GetViewportRect() const noexcept;
	void SetViewportRect(const Vector4D& viewportRect_) noexcept;

	[[nodiscard]] Matrix4x4 GetViewMatrix() const;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix(float aspectRatio_) const;

	[[nodiscard]] Matrix4x4 GetProjectionMatrix(float aspectRatio_) const;

protected:
	virtual void OnAttach() override;
	virtual void OnDetach() override;

private:
	std::unique_ptr<CameraProjection> projection;
	float nearPlane{ 0.1f };
	float farPlane{ 100.0f };
	Vector4D viewportRect{ 0.0f, 0.0f, 1.0f, 1.0f };
};
