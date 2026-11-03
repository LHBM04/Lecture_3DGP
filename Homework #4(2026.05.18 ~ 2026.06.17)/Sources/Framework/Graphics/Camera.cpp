#include "Precompiled.hpp"
#include "Camera.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"
#include "../Entities/World.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

namespace TUK::Framework
{
	Camera::~Camera() noexcept
	{
		// Entity가 직접 해제되는 경우에도 비소유 목록에 주소가 남지 않게 합니다.
		GetOwner().GetWorld().RemoveCamera(*this);
	}

	void Camera::OnEnable()
	{
		GetOwner().GetWorld().AddCamera(*this);
	}

	void Camera::OnDisable()
	{
		GetOwner().GetWorld().RemoveCamera(*this);
	}

	Matrix4x4<float> Camera::GetViewMatrix() const noexcept
	{
		const Transform* transform = GetOwner().GetComponent<Transform>();
		if (!transform)
		{
			return Matrix4x4<float>::GetIdentity();
		}

		const Vector3D<float> position = transform->GetWorldPosition();
		const Quaternion<float> rotation = transform->GetWorldRotation();

		const Matrix4x4<float> translationMatrix = Matrix4x4<float>::Translate(-position);
		const Matrix4x4<float> rotationMatrix = Matrix4x4<float>::Rotate(Quaternion<float>::Inverse(rotation));

		// 행 벡터 규칙: 카메라 위치를 제거한 뒤 카메라 회전의 역회전을 적용합니다.
		return translationMatrix * rotationMatrix;
	}

	Matrix4x4<float> Camera::GetProjectionMatrix() const noexcept
	{
		assert(viewport.Width > 0.0f && viewport.Height > 0.0f);
		const float aspectRatio = viewport.Width / viewport.Height;

		switch (projectionType)
		{
			case CameraProjectionType::Orthographic:
			{
				const float halfHeight = orthoSize * 0.5f;
				const float halfWidth = halfHeight * aspectRatio;
				
				return Matrix4x4<float>::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
			}
			case CameraProjectionType::Perspective:
			{
				return Matrix4x4<float>::Perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
			}
			default:
			{
				return Matrix4x4<float>::GetIdentity();
			}
		}
	}

	Matrix4x4<float> Camera::GetViewProjectionMatrix() const noexcept
	{
		return GetViewMatrix() * GetProjectionMatrix();
	}
}
