#pragma once

#include "Matrix4x4.h"

class CameraProjection
{
public:
	virtual ~CameraProjection() noexcept = default;

	CameraProjection(const CameraProjection&) = delete;
	CameraProjection& operator=(const CameraProjection&) = delete;

	CameraProjection(CameraProjection&&) = delete;
	CameraProjection& operator=(CameraProjection&&) = delete;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const = 0;

protected:
	CameraProjection() noexcept = default;
};
