#include "Precompiled.hpp"
#include "Transition.hpp"

#include <algorithm>
#include <cmath>
#include <span>
#include <utility>

#include <directx/d3dx12.h>

#include "Framework/Graphics/GPUCommandBuffer.hpp"
#include "Framework/Graphics/GPUDevice.hpp"
#include "Framework/Graphics/GPUTarget.hpp"

#include "Framework/Resources/ShaderAssetLoader.hpp"

namespace TUK::Game
{
	using namespace Framework;

	std::expected<void, std::string> Transition::Initialize(
		const GPUDevice& device, AssetSubsystem* assets)
	{
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

		pipeline = std::move(*created);
		return {};
	}

	void Transition::Shutdown() noexcept
	{
		Reset();
		pipeline.reset();
	}

	void Transition::Reset() noexcept
	{
		state = TransitionState::IDLE;
		opacity = 0.0f;
		hasRenderedExit = false;
	}

	bool Transition::Enter(f32 duration) noexcept
	{
		if (!pipeline || !std::isfinite(duration) || duration <= 0.0f)
		{
			return false;
		}
		this->duration = duration;
		state = TransitionState::ENTER;
		return true;
	}

	bool Transition::Exit(f32 duration) noexcept
	{
		if (!pipeline || !std::isfinite(duration) || duration <= 0.0f)
		{
			return false;
		}
		this->duration = duration;
		hasRenderedExit = false;
		state = TransitionState::EXIT;
		return true;
	}

	void Transition::Update(f32 deltaTime)
	{
		if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
		{
			return;
		}
		switch (state)
		{
			case TransitionState::ENTER:
			{
				opacity = std::max(opacity - deltaTime / duration, 0.0f);
				if (opacity == 0.0f)
				{
					state = TransitionState::ENTERCOMPLETE;
				}
				break;
			}
			case TransitionState::EXIT:
			{
				opacity = std::min(opacity + deltaTime / duration, 1.0f);
				if (opacity == 1.0f)
				{
					state = TransitionState::EXITCOMPLETE;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	std::expected<void, std::string> Transition::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!pipeline || opacity <= 0.0f)
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
		if (state == TransitionState::EXITCOMPLETE)
		{
			hasRenderedExit = true;
		}
		return {};
	}
}
