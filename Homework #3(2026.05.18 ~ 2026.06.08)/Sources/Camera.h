#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "Matrix4x4.h"
#include "Vector4D.h"

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

	Camera() noexcept = default;
	~Camera() noexcept override = default;

	[[nodiscard]] float GetNearClipPlane() const noexcept;
	void SetNearClipPlane(float nearClipPlane_) noexcept;

	[[nodiscard]] float GetFarClipPlane() const noexcept;
	void SetFarClipPlane(float farClipPlane_) noexcept;

	[[nodiscard]] const Vector4D& GetViewport() const noexcept;
	void SetViewport(const Vector4D& viewport_) noexcept;

	[[nodiscard]] ProjectionType GetProjectionType() const noexcept;
	void SetProjectionType(ProjectionType projectionType_) noexcept;

	[[nodiscard]] float GetFieldOfView() const noexcept;
	void SetFieldOfView(float fieldOfViewDegrees_) noexcept;

	[[nodiscard]] float GetOrthographicSize() const noexcept;
	void SetOrthographicSize(float orthographicSize_) noexcept;

	[[nodiscard]] float GetAspectRatio() const noexcept;
	void SetAspectRatio(float aspectRatio_) noexcept;

	[[nodiscard]] ClearType GetClearMode() const noexcept;
	void SetClearMode(ClearType clearMode_) noexcept;

	[[nodiscard]] const ColorRGBA& GetClearColor() const noexcept;
	void SetClearColor(const ColorRGBA& clearColor_) noexcept;

	[[nodiscard]] Matrix4x4 GetViewMatrix() const noexcept;
	[[nodiscard]] Matrix4x4 GetProjectionMatrix() const noexcept;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix() const noexcept;

	void OnEnable() override;
	void OnDisable() override;

private:
	Vector4D viewportRect{ 0.0f, 0.0f, 1.0f, 1.0f };
	ProjectionType projectionType{ ProjectionType::Perspective };
	float fieldOfViewDegrees{ 60.0f };
	float orthographicSize{ 5.0f };
	float aspectRatio{ 4.0f / 3.0f };
	float nearClipPlane{ 0.1f };
	float farClipPlane{ 100.0f };
	ClearType clearType{ ClearType::SolidColor };
	ColorRGBA clearColor{ ColorRGBA::GetBlue() };
};
