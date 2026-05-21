#pragma once

#include "CameraProjection.h"

class CameraProjection_Orthographic final : public CameraProjection
{
public:
	CameraProjection_Orthographic() noexcept = default;
	explicit CameraProjection_Orthographic(float orthographicSize_) noexcept;

	[[nodiscard]] float GetOrthographicSize() const noexcept;
	void SetOrthographicSize(float orthographicSize_) noexcept;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const override;

private:
	float orthographicSize{ 5.0f };
};
