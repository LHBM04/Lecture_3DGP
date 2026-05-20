#pragma once

#include "CameraProjection.h"

class OrthographicCameraProjection final : public CameraProjection
{
public:
	OrthographicCameraProjection() noexcept = default;
	explicit OrthographicCameraProjection(float orthographicSize_) noexcept;

	[[nodiscard]] float GetOrthographicSize() const noexcept;
	void SetOrthographicSize(float orthographicSize_) noexcept;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const override;

private:
	float orthographicSize{ 5.0f };
};
