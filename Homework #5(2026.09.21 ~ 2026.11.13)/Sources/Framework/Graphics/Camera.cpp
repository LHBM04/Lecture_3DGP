#include "Precompiled.hpp"
#include "Camera.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <span>

#include "../Core/Common.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"
#include "../Entities/World.hpp"

#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "../Resources/Cubemap.hpp"
#include "../Resources/MaterialAsset.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUTarget.hpp"

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

	std::expected<void, std::string> Camera::PrepareRender(GPUCommandBuffer& commandBuffer) const
	{
		if (commandBuffer.GetActiveTargetFormat() == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("카메라를 적용하기 전에 렌더 타깃을 설정해야 합니다.");
		}

		struct CameraConstants final
		{
			DirectX::XMFLOAT4X4 view;
			DirectX::XMFLOAT4X4 projection;
		};

		CameraConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<f32>::Load(GetViewMatrix()));
		DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<f32>::Load(GetProjectionMatrix()));

		constexpr u32 CAMERA_ROOT_SLOT = 0;
		if (auto result = commandBuffer.SetGraphicsConstants(CAMERA_ROOT_SLOT, std::as_bytes(std::span(&constants, 1))); !result)
		{
			return result;
		}

		commandBuffer.SetViewport(viewport);
		commandBuffer.SetScissorRect(scissorRect);
		return {};
	}

	Camera::BackgroundPass Camera::PrepareBackground(
		GPUCommandBuffer& commandBuffer, const MaterialAsset* skyboxMaterial) const
	{
		switch (clearFlags)
		{
			case CameraClearFlags::SolidColor:
			{
				ClearBuffers(commandBuffer, true);
				break;
			}
			case CameraClearFlags::DepthOnly:
			{
				ClearBuffers(commandBuffer, false);
				break;
			}
			case CameraClearFlags::Skybox:
			{
				const bool canDrawSkybox = cubemap && skyboxMaterial && skyboxMaterial->GetPass() == MaterialPass::SKYBOX &&
					skyboxMaterial->GetTargetFormat() == commandBuffer.GetActiveTargetFormat();
				if (ClearBuffers(commandBuffer, true) && canDrawSkybox)
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
		if (!pass.material || !pass.cubemap ||
			pass.material->GetPass() != MaterialPass::SKYBOX ||
			pass.material->GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
		{
			return;
		}

		const GPUTexture2D& texture = pass.cubemap->GetTexture();
		const auto description = texture.GetD3DResource().GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC view{};
		view.Format = description.Format;
		view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		view.TextureCube.MipLevels = description.MipLevels;

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &texture };
		if (auto result = commandBuffer.BindTextures(textures, &view); !result)
		{
			LOG_ERROR("스카이박스 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(pass.material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		constexpr u32 FULLSCREEN_VERTEX_COUNT = 3;
		commandBuffer.Draw(FULLSCREEN_VERTEX_COUNT);
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

	bool Camera::ClearBuffers(GPUCommandBuffer& commandBuffer, bool clearColor) const
	{
		const D3D12_RECT& bounds = commandBuffer.GetActiveTargetBounds();
		const f32 targetWidth = static_cast<f32>(bounds.right);
		const f32 targetHeight = static_cast<f32>(bounds.bottom);
		const D3D12_RECT rect{
			std::max(scissorRect.left, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftX, 0.0f, targetWidth)))),
			std::max(scissorRect.top, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftY, 0.0f, targetHeight)))),
			std::min(scissorRect.right, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftX + viewport.Width, 0.0f, targetWidth)))),
			std::min(scissorRect.bottom, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftY + viewport.Height, 0.0f, targetHeight))))
		};
		if (rect.left >= rect.right || rect.top >= rect.bottom)
		{
			return false;
		}

		if (clearColor)
		{
			const f32 color[] = { backgroundColor.GetR(), backgroundColor.GetG(), backgroundColor.GetB(), backgroundColor.GetA() };
			commandBuffer.ClearRenderTarget(color, rect);
		}
		commandBuffer.ClearDepth(GPUTarget::DEPTH_CLEAR_VALUE, rect);
		return true;
	}
}
