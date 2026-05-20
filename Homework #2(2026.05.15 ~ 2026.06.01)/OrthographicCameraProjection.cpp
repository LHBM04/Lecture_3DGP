#include "Precompiled.h"
#include "OrthographicCameraProjection.h"

OrthographicCameraProjection::OrthographicCameraProjection(float orthographicSize_) noexcept
{
	SetOrthographicSize(orthographicSize_);
}

float OrthographicCameraProjection::GetOrthographicSize() const noexcept
{
	return orthographicSize;
}

void OrthographicCameraProjection::SetOrthographicSize(float orthographicSize_) noexcept
{
	orthographicSize = std::max(orthographicSize_, Mathf::EPSILON);
}

Matrix4x4 OrthographicCameraProjection::GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const
{
	const float height{ orthographicSize * 2.0f };
	return Matrix4x4::Ortho(height * aspectRatio_, height, nearPlane_, farPlane_);
}
