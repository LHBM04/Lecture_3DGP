#include "Precompiled.h"
#include "CameraProjection_Orthographic.h"

#include "Mathf.h"
#include <algorithm>

CameraProjection_Orthographic::CameraProjection_Orthographic(float orthographicSize_) noexcept
{
	SetOrthographicSize(orthographicSize_);
}

float CameraProjection_Orthographic::GetOrthographicSize() const noexcept
{
	return orthographicSize;
}

void CameraProjection_Orthographic::SetOrthographicSize(float orthographicSize_) noexcept
{
	orthographicSize = std::max(orthographicSize_, Mathf::EPSILON);
}

Matrix4x4 CameraProjection_Orthographic::GetProjectionMatrix(float aspectRatio_, float nearPlane_, float farPlane_) const
{
	const float height{ orthographicSize * 2.0f };
	return Matrix4x4::Ortho(height * aspectRatio_, height, nearPlane_, farPlane_);
}
