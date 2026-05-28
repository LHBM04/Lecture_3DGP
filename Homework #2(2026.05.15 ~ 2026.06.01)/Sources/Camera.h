#pragma once

#include "ColorRGB.h"
#include "ColorRGBA.h"
#include "Component.h"
#include "Matrix4x4.h"
#include "Vector4D.h"

class CameraProjection;

class Camera : public Component
{
public:
	enum class Projection : unsigned char
	{
		Persprective,	// 원근 투영
		Orthographic	// 직교 투영
	};

	enum class ClearMode : unsigned char
	{
		SolidColor,
		OnlyDepth,
		None,
	};

	[[nodiscard]] CameraProjection* GetProjection() noexcept;
	[[nodiscard]] const CameraProjection* GetProjection() const noexcept;
	void SetProjection(Projection projection_) noexcept;

	[[nodiscard]] float GetNearClipPlane() const noexcept;
	void SetNearClipPlane(float nearClipPlane_) noexcept;

	[[nodiscard]] float GetFarClipPlane() const noexcept;
	void SetFarClipPlane(float farClipPlane_) noexcept;

	[[nodiscard]] const Vector4D& GetViewport() const noexcept;
	void SetViewport(const Vector4D& viewport_) noexcept;

	[[nodiscard]] ClearMode GetClearMode() const noexcept;
	void SetClearMode(ClearMode clearMode_) noexcept;

	[[nodiscard]] const ColorRGBA& GetClearColor() const noexcept;
	void SetClearColor(const ColorRGBA& clearColor_) noexcept;

	[[nodiscard]] Matrix4x4 GetViewMatrix() const;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix(float aspectRatio_) const;

	[[nodiscard]] Matrix4x4 GetProjectionMatrix(float aspectRatio_) const;
	// void GetWorldFrustum(float aspectRatio_, DirectX::BoundingFrustum& outFrustum_) const;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	std::unique_ptr<CameraProjection> projection;
	float nearPlane{ 0.1f };
	float farPlane{ 100.0f };
	Vector4D viewportRect{ 0.0f, 0.0f, 1.0f, 1.0f };
	ClearMode clearMode{ ClearMode::SolidColor };
	ColorRGBA clearColor{ ColorRGBA::GetBlue() };
};
