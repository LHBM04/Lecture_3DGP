#pragma once

#include "CameraProjection.h"

class PerspectiveCameraProjection final : public CameraProjection
{
public:
	PerspectiveCameraProjection() noexcept = default;
	explicit PerspectiveCameraProjection(float fieldOfView_) noexcept;

	[[nodiscard]] float GetFieldOfView() const noexcept;
	void SetFieldOfView(float fieldOfView_) noexcept;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const override;

private:
	float fieldOfView{ 60.0f };
};
