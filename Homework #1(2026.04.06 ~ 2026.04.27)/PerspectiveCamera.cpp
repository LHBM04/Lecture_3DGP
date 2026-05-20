#include "Precompiled.hpp"
#include "PerspectiveCamera.hpp"

float PerspectiveCamera::GetFieldOfView() const
{
	return fieldOfViewDegrees;
}

void PerspectiveCamera::SetFieldOfView(float fieldOfViewDegrees_)
{
	fieldOfViewDegrees = Mathf::Clamp(fieldOfViewDegrees_, 1.0f, 179.0f);
}

Matrix4x4 PerspectiveCamera::GetProjectionMatrix(float aspectRatio_) const
{
	const float validAspectRatio = std::max(aspectRatio_, Mathf::EPSILON);
	return Matrix4x4::Perspective(fieldOfViewDegrees, validAspectRatio, GetNearClipPlane(), GetFarClipPlane());
}
