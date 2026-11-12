#include "Precompiled.hpp"
#include "PlayerCameraController.hpp"

#include "Framework/Graphics.hpp"
#include "Framework/Math.hpp"
#include "Framework/Resources.hpp"

#include "../Core/Assets.hpp"
#include "../Core/Input.hpp"
#include "../Core/Time.hpp"

namespace TUK::Game
{
	void PlayerCameraController::OnStart()
	{
		crosshairTexture = Assets::LoadAsset<TextureAsset>("Resources/Textures/Crosshairs/Outline/crosshair-010.dds");
		crosshairMaterial = Assets::LoadAsset<MaterialAsset>("Resources/Materials/ImageUI.tukmat");
	}

	void PlayerCameraController::OnLateUpdate()
	{
		if (!target || target->IsDestroyed() || target->GetOwner()->IsDestroyed())
		{
			return;
		}

		Transform* cameraTransform = GetOwner()->GetComponent<Transform>();
		if (!cameraTransform || cameraTransform == target)
		{
			return;
		}

		if (Input::IsKeyPressed(KeyCode::ENTER))
		{
			isFirstPerson = !isFirstPerson;
		}

		const Quaternion<f32> targetRotation = target->GetWorldRotation();
		if (isFirstPerson)
		{
			cameraTransform->SetWorldPosition(target->GetWorldPosition() + targetRotation * firstPersonOffset);
			cameraTransform->SetWorldRotation(targetRotation);
			return;
		}

		const Vector3D<f32> offset = targetRotation * Vector3D<f32>(0.0f, followHeight, -followDistance);
		const Vector3D<f32> up = targetRotation * Vector3D<f32>::GetUp();
		const Vector3D<f32> desiredPosition = target->GetWorldPosition() + offset;
		const Quaternion<f32> desiredRotation = Quaternion<f32>::LookRotation(-offset, up);

		if (isFirstFrame)
		{
			cameraTransform->SetWorldPosition(desiredPosition);
			cameraTransform->SetWorldRotation(desiredRotation);
			isFirstFrame = false;
			return;
		}

		const f32 deltaTime = Time::GetDeltaTime();
		const f32 t = Mathf::Clamp(smoothSpeed * deltaTime, 0.0f, 1.0f);

		const Vector3D<f32> currentPosition = cameraTransform->GetWorldPosition();
		const Vector3D<f32> newPosition
		{
			Mathf::Lerp(currentPosition.GetX(), desiredPosition.GetX(), t),
			Mathf::Lerp(currentPosition.GetY(), desiredPosition.GetY(), t),
			Mathf::Lerp(currentPosition.GetZ(), desiredPosition.GetZ(), t)
		};

		const Quaternion<f32> currentRotation = cameraTransform->GetWorldRotation();
		const Quaternion<f32> newRotation = Quaternion<f32>::Slerp(currentRotation, desiredRotation, t);

		cameraTransform->SetWorldPosition(newPosition);
		cameraTransform->SetWorldRotation(newRotation);
	}

	void PlayerCameraController::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!isFirstPerson || !target || !crosshairTexture || !crosshairMaterial)
		{
			return;
		}

		const D3D12_RECT& bounds = commandBuffer.GetActiveTargetBounds();
		const f32 width = static_cast<f32>(bounds.right - bounds.left);
		const f32 height = static_cast<f32>(bounds.bottom - bounds.top);
		if (width <= 0.0f || height <= 0.0f)
		{
			return;
		}

		const CD3DX12_VIEWPORT viewport(
			static_cast<f32>(bounds.left), static_cast<f32>(bounds.top), width, height);
		commandBuffer.SetViewport(viewport);
		commandBuffer.SetScissorRect(bounds);

		const ObjectConstants objectConstants
		{
			.world = Matrix4x4<f32>::Scale(Vector3D<f32>(CROSSHAIR_SIZE, CROSSHAIR_SIZE, 1.0f))
		};
		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			LOG_ERROR("조준선 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const ImageUIConstants constants
		{
			.viewportSize = Vector2D<f32>{ width, height },
			.color = ColorRGBA<f32>::GetWhite()
		};
		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
		{
			LOG_ERROR("조준선 UI 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &crosshairTexture->GetTexture() };
		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("조준선 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(crosshairMaterial->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(crosshairMaterial->GetPrimitive());
		constexpr u32 CROSSHAIR_VERTEX_COUNT = 4;
		commandBuffer.Draw(CROSSHAIR_VERTEX_COUNT);
	}
}
