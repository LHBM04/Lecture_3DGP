#include "Precompiled.hpp"
#include "OrthographicsCamera.hpp"

float OrthographicsCamera::GetOrthographicSize() const
{
	return orthographicSize;
}

void OrthographicsCamera::SetOrthographicSize(float orthographicSize_)
{
	orthographicSize = std::max(orthographicSize_, Mathf::EPSILON);
}

Matrix4x4 OrthographicsCamera::GetProjectionMatrix(float aspectRatio_) const
{
	const float validAspectRatio = std::max(aspectRatio_, Mathf::EPSILON);
	const float halfHeight = orthographicSize;
	const float halfWidth = halfHeight * validAspectRatio;
	return Matrix4x4::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, GetNearClipPlane(), GetFarClipPlane());
}
