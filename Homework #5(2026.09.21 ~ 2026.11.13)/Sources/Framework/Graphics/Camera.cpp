#include "Precompiled.hpp"
#include "Camera.hpp"

namespace TUK::Framework
{
	Matrix4x4<f32> Camera::GetViewMatrix() const noexcept
	{
		return Matrix4x4<f32>::GetIdentity();
	}

	Matrix4x4<f32> Camera::GetProjectionMatrix() const noexcept
	{
		return Matrix4x4<f32>::GetIdentity();
	}
}
