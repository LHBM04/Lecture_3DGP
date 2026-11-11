#include "Precompiled.hpp"
#include "WorldTransition.hpp"

#include <algorithm>
#include <cmath>
#include <span>
#include <utility>

#include <directx/d3dx12.h>

#include "Framework/Entities/World.hpp"
#include "Framework/Entities/WorldSubsystem.hpp"

#include "Framework/Graphics/GPUCommandBuffer.hpp"
#include "Framework/Graphics/GPUDevice.hpp"
#include "Framework/Graphics/GPUTarget.hpp"

#include "Framework/Platform/InputSubsystem.hpp"

#include "Framework/Resources/ShaderAssetLoader.hpp"

namespace TUK::Game
{
	using namespace Framework;

	std::expected<void, std::string> WorldTransition::Initialize(
		WorldSubsystem* worlds, InputSubsystem* input, const GPUDevice& device, AssetSubsystem* assets)
	{
		ASSERT_FMT(worlds, "월드 서브시스템이 nullptr입니다.");
		ASSERT_FMT(input, "입력 서브시스템이 nullptr입니다.");
		ASSERT_FMT(assets, "에셋 서브시스템이 nullptr입니다.");

		// Load directly so world asset cleanup cannot release the transition pipeline.
		ShaderAssetLoader loader(assets);
		auto vertex = loader.Load("Resources/Shaders/WorldTransition.vert.cso");
		if (!vertex)
		{
			return std::unexpected(vertex.error());
		}
		auto pixel = loader.Load("Resources/Shaders/WorldTransition.frag.cso");
		if (!pixel)
		{
			return std::unexpected(pixel.error());
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.VS = (*vertex)->GetShader().GetD3DBytecode();
		description.PS = (*pixel)->GetShader().GetD3DBytecode();
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = TARGET_FORMAT;
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DepthStencilState.DepthEnable = FALSE;
		description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		auto& blend = description.BlendState.RenderTarget[0];
		blend.BlendEnable = TRUE;
		blend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blend.SrcBlendAlpha = D3D12_BLEND_ONE;
		blend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;

		auto created = device.CreateGraphicsPipelineState(description);
		if (!created)
		{
			return std::unexpected(created.error());
		}

		this->worlds = worlds;
		this->input = input;
		pipeline = std::move(*created);
		return {};
	}

	void WorldTransition::Shutdown() noexcept
	{
		Finish();
		pipeline.reset();
		worlds = nullptr;
		input = nullptr;
	}

	bool WorldTransition::Request(World* world, f32 duration) noexcept
	{
		if (!pipeline || IsActive() || !world || world == worlds->GetCurrentWorld() ||
			!std::isfinite(duration) || duration <= 0.0f)
		{
			return false;
		}

		targetWorld = world;
		fadeDuration = duration;
		opacity = 0.0f;
		hasRenderedBlackFrame = false;
		wasInputEnabled = input->IsInputEnabled();
		input->SetInputEnabled(false);
		phase = Phase::FADE_OUT;
		return true;
	}

	void WorldTransition::Update(f32 deltaTime)
	{
		if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
		{
			return;
		}

		switch (phase)
		{
			case Phase::IDLE:
			{
				break;
			}
			case Phase::FADE_OUT:
			{
				opacity = std::min(opacity + deltaTime / fadeDuration, 1.0f);
				if (opacity == 1.0f)
				{
					phase = Phase::BLACK_FRAME;
				}
				break;
			}
			case Phase::BLACK_FRAME:
			{
				// Render runs after Update. The next Update follows submission and presentation.
				if (hasRenderedBlackFrame)
				{
					if (worlds->RequestWorldChange(targetWorld))
					{
						phase = Phase::WAIT_FOR_WORLD;
					}
					else
					{
						LOG_ERROR("페이드 후 월드 전환을 예약하지 못했습니다.");
						phase = Phase::FADE_IN;
					}
				}
				break;
			}
			case Phase::WAIT_FOR_WORLD:
			{
				const World* currentWorld = worlds->GetCurrentWorld();
				if (currentWorld && currentWorld->IsEntered())
				{
					if (currentWorld != targetWorld)
					{
						LOG_WARNING("페이드 중 전환 대상이 변경되어 현재 월드를 표시합니다.");
					}
					phase = Phase::FADE_IN;
				}
				break;
			}
			case Phase::FADE_IN:
			{
				opacity = std::max(opacity - deltaTime / fadeDuration, 0.0f);
				if (opacity == 0.0f)
				{
					Finish();
				}
				break;
			}
		}
	}

	std::expected<void, std::string> WorldTransition::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!IsActive() || opacity <= 0.0f)
		{
			return {};
		}
		if (commandBuffer.GetActiveTargetFormat() != TARGET_FORMAT)
		{
			return std::unexpected("페이드 파이프라인과 렌더 타깃의 형식이 다릅니다.");
		}

		const D3D12_RECT& bounds = commandBuffer.GetActiveTargetBounds();
		const CD3DX12_VIEWPORT viewport(
			static_cast<f32>(bounds.left), static_cast<f32>(bounds.top),
			static_cast<f32>(bounds.right - bounds.left), static_cast<f32>(bounds.bottom - bounds.top));
		commandBuffer.SetViewport(viewport);
		commandBuffer.SetScissorRect(bounds);

		const DirectX::XMFLOAT4 color{ 0.0f, 0.0f, 0.0f, opacity };
		commandBuffer.SetGraphicsRootConstants(
			GPUCommandBuffer::ROOT_CONSTANT_SLOT, std::as_bytes(std::span(&color, 1)));

		commandBuffer.SetGPUGraphicsPipelineState(*pipeline);
		commandBuffer.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		constexpr u32 FULLSCREEN_VERTEX_COUNT = 3;
		commandBuffer.Draw(FULLSCREEN_VERTEX_COUNT);
		if (phase == Phase::BLACK_FRAME)
		{
			hasRenderedBlackFrame = true;
		}
		return {};
	}

	void WorldTransition::Finish() noexcept
	{
		if (IsActive())
		{
			input->SetInputEnabled(wasInputEnabled);
		}
		phase = Phase::IDLE;
		targetWorld = nullptr;
		opacity = 0.0f;
		hasRenderedBlackFrame = false;
	}
}
