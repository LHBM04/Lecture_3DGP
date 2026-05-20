#include "Precompiled.h"
#include "PerspectiveCameraProjection.h"

PerspectiveCameraProjection::PerspectiveCameraProjection(float fieldOfView_) noexcept
{
	SetFieldOfView(fieldOfView_);
}

float PerspectiveCameraProjection::GetFieldOfView() const noexcept
{
	return fieldOfView;
}

void PerspectiveCameraProjection::SetFieldOfView(float fieldOfView_) noexcept
{
	fieldOfView = std::clamp(fieldOfView_, 1.0f, 179.0f);
}

Matrix4x4 PerspectiveCameraProjection::GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const
{
	return Matrix4x4::Perspective(fieldOfView, aspectRatio_, nearPlane_, farPlane_);
}
