#include "Precompiled.hpp"
#include "ImageView.hpp"

#include <array>
#include <span>

#include "../Core/App.hpp"
#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"

#include "../Resources/AssetSubsystem.hpp"
#include "../Resources/MaterialAsset.hpp"
#include "../Resources/TextureAsset.hpp"

namespace TUK::Framework
{
	void ImageView::OnStart()
	{
		AssetSubsystem* assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			LOG_ERROR("UI 머티리얼을 로드할 에셋 서브시스템이 없습니다.");
			return;
		}

		material = assets->LoadAsset<MaterialAsset>("Resources/Materials/ImageUI.tukmaterial");
		if (!material || material->GetPass() != MaterialPass::IMAGE_UI)
		{
			LOG_ERROR("ImageUI 머티리얼을 로드하지 못했습니다.");
			material = nullptr;
		}
	}

	void ImageView::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!image || !material)
		{
			return;
		}

		const D3D12_VIEWPORT& viewport = commandBuffer.GetActiveViewport();
		if (material->GetTargetFormat() != commandBuffer.GetActiveTargetFormat() ||
			viewport.Width <= 0.0f || viewport.Height <= 0.0f)
		{
			return;
		}

		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			LOG_ERROR("UI 이미지 엔티티에 Transform 컴포넌트가 필요합니다.");
			return;
		}

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, Matrix4x4<f32>::Load(transform->GetWorldMatrix()));

		constexpr u32 OBJECT_ROOT_SLOT = 1;
		if (auto result = commandBuffer.SetGraphicsConstants(OBJECT_ROOT_SLOT, std::as_bytes(std::span(&world, 1))); !result)
		{
			LOG_ERROR("UI 이미지 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		struct ImageUIConstants final
		{
			DirectX::XMFLOAT2 viewportSize;
			DirectX::XMFLOAT2 padding{};
		};
		static_assert(sizeof(ImageUIConstants) == 16);

		const ImageUIConstants constants{ { viewport.Width, viewport.Height } };
		constexpr u32 IMAGE_UI_ROOT_SLOT = 2;
		if (auto result = commandBuffer.SetGraphicsConstants(IMAGE_UI_ROOT_SLOT, std::as_bytes(std::span(&constants, 1))); !result)
		{
			LOG_ERROR("UI 뷰포트 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{ &image->GetTexture() };
		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("UI 이미지 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		constexpr u32 IMAGE_VERTEX_COUNT = 4;
		commandBuffer.Draw(IMAGE_VERTEX_COUNT);
	}
}
