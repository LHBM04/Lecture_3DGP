#include "Precompiled.hpp"
#include "Camera.hpp"

#include "../Core/Common.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"
#include "../Entities/World.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "../Resources/MaterialAsset.hpp"

#include "GPUCommandBuffer.hpp"

namespace TUK::Framework
{
	void Camera::OnEnable()
	{
		GetOwner()->GetWorld()->AddCamera(this);
	}

	void Camera::OnDisable()
	{
		GetOwner()->GetWorld()->RemoveCamera(this);
	}

	Camera::BackgroundPass Camera::PrepareBackground(
		GPUCommandBuffer& commandBuffer, const MaterialAsset* skyboxMaterial) const
	{
		switch (clearFlags)
		{
			case CameraClearFlags::SolidColor:
			{
				commandBuffer.ClearCameraBuffers(this, true);
				break;
			}
			case CameraClearFlags::DepthOnly:
			{
				commandBuffer.ClearCameraBuffers(this, false);
				break;
			}
			case CameraClearFlags::Skybox:
			{
				const bool canDrawSkybox = cubemap && skyboxMaterial && skyboxMaterial->IsSkybox() &&
					skyboxMaterial->GetTargetFormat() == commandBuffer.GetActiveTargetFormat();
				if (commandBuffer.ClearCameraBuffers(this, !canDrawSkybox) && canDrawSkybox)
				{
					return { skyboxMaterial, cubemap };
				}
				break;
			}
			case CameraClearFlags::Nothing:
			{
				break;
			}
		}
		return {};
	}

	void Camera::RenderBackground(GPUCommandBuffer& commandBuffer, const BackgroundPass& pass) const
	{
		if (!pass.material)
		{
			return;
		}

		if (auto result = commandBuffer.DrawSkybox(pass.material, pass.cubemap); !result)
		{
			LOG_ERROR("스카이박스 렌더링에 실패했습니다: {}", result.error());
		}
	}

	Matrix4x4<f32> Camera::GetViewMatrix() const noexcept
	{
		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			return Matrix4x4<f32>::GetIdentity();
		}

		const Vector3D<f32> position = transform->GetWorldPosition();
		const Quaternion<f32> rotation = transform->GetWorldRotation();

		const Matrix4x4<f32> translationMatrix = Matrix4x4<f32>::Translate(-position);
		const Matrix4x4<f32> rotationMatrix = Matrix4x4<f32>::Rotate(Quaternion<f32>::Inverse(rotation));

		// 행 벡터 규칙: 카메라 위치를 제거한 뒤 카메라 회전의 역회전을 적용합니다.
		return translationMatrix * rotationMatrix;
	}

	Matrix4x4<f32> Camera::GetProjectionMatrix() const noexcept
	{
		ASSERT_FMT(viewport.Width > 0.0f, "viewport.Width가 0보다 커야 합니다.");
		ASSERT_FMT(viewport.Height > 0.0f, "viewport.Height가 0보다 커야 합니다.");

		const f32 aspectRatio = viewport.Width / viewport.Height;

		switch (projectionType)
		{
			case CameraProjectionType::Orthographic:
			{
				const f32 halfHeight = orthoSize * 0.5f;
				const f32 halfWidth = halfHeight * aspectRatio;

				return Matrix4x4<f32>::Ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
			}
			case CameraProjectionType::Perspective:
			{
				return Matrix4x4<f32>::Perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
			}
			default:
			{
				return Matrix4x4<f32>::GetIdentity();
			}
		}
	}
}
