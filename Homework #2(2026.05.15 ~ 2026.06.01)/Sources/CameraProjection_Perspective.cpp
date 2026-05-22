#include "Precompiled.h"
#include "CameraProjection_Perspective.h"

CameraProjection_Perspective::CameraProjection_Perspective(float fieldOfView_) noexcept
{
	SetFieldOfView(fieldOfView_);
}

float CameraProjection_Perspective::GetFieldOfView() const noexcept
{
	return fieldOfView;
}

void CameraProjection_Perspective::SetFieldOfView(float fieldOfView_) noexcept
{
	fieldOfView = std::clamp(fieldOfView_, 1.0f, 179.0f);
}

Matrix4x4 CameraProjection_Perspective::GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const
{
	return Matrix4x4::Perspective(fieldOfView, aspectRatio_, nearPlane_, farPlane_);
}
