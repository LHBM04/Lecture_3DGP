#pragma once

#include "CameraProjection.h"

class CameraProjection_Perspective final : public CameraProjection
{
public:
	CameraProjection_Perspective() noexcept = default;
	explicit CameraProjection_Perspective(float fieldOfView_) noexcept;

	[[nodiscard]] float GetFieldOfView() const noexcept;
	void SetFieldOfView(float fieldOfView_) noexcept;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const override;

private:
	float fieldOfView{ 60.0f };
};
