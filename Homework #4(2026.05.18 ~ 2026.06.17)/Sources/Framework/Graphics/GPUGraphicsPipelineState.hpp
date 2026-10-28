#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <span>
#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPURootSignature;
	class GPUShader;

	struct GPUGraphicsPipelineStateCreateInfo final
	{
		const GPURootSignature& rootSignature;
		const GPUShader& vertexShader;
		const GPUShader* pixelShader = nullptr;

		// 입력 배열, SemanticName과 셰이더는 생성 호출이 끝날 때까지 유효해야 합니다.
		std::span<const D3D12_INPUT_ELEMENT_DESC> inputElements;
		std::span<const DXGI_FORMAT> renderTargetFormats;
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT_UNKNOWN;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		D3D12_INDEX_BUFFER_STRIP_CUT_VALUE stripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		std::uint32_t sampleMask = (std::numeric_limits<std::uint32_t>::max)();
		DXGI_SAMPLE_DESC sampleDescription{ 1, 0 };

		D3D12_RASTERIZER_DESC rasterizerState
		{
			.FillMode = D3D12_FILL_MODE_SOLID,
			.CullMode = D3D12_CULL_MODE_BACK,
			.FrontCounterClockwise = FALSE,
			.DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
			.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
			.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
			.DepthClipEnable = TRUE,
		};

		D3D12_BLEND_DESC blendState = []
		{
			D3D12_BLEND_DESC description{};
			for (D3D12_RENDER_TARGET_BLEND_DESC& target : description.RenderTarget)
			{
				target.SrcBlend = D3D12_BLEND_ONE;
				target.DestBlend = D3D12_BLEND_ZERO;
				target.BlendOp = D3D12_BLEND_OP_ADD;
				target.SrcBlendAlpha = D3D12_BLEND_ONE;
				target.DestBlendAlpha = D3D12_BLEND_ZERO;
				target.BlendOpAlpha = D3D12_BLEND_OP_ADD;
				target.LogicOp = D3D12_LOGIC_OP_NOOP;
				target.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			}
			return description;
		}();

		// 기본값은 깊이/스텐실 비활성화입니다. 사용 시 형식과 활성화 상태를 함께 지정합니다.
		D3D12_DEPTH_STENCIL_DESC depthStencilState
		{
			.DepthEnable = FALSE,
			.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
			.StencilEnable = FALSE,
			.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
			.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
			.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS },
			.BackFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS },
		};
	};

	class GPUGraphicsPipelineState
	{
		friend class GPUDevice;

	public:
		~GPUGraphicsPipelineState() noexcept = default;

		GPUGraphicsPipelineState(const GPUGraphicsPipelineState&) = delete;
		GPUGraphicsPipelineState& operator=(const GPUGraphicsPipelineState&) = delete;

		GPUGraphicsPipelineState(GPUGraphicsPipelineState&&) noexcept = default;
		GPUGraphicsPipelineState& operator=(GPUGraphicsPipelineState&&) noexcept = default;

		[[nodiscard]] ID3D12PipelineState& GetD3DPipelineState() const noexcept;

	private:
		explicit GPUGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
	};

	inline ID3D12PipelineState& GPUGraphicsPipelineState::GetD3DPipelineState() const noexcept
	{
		assert(d3dPipelineState);
		return *d3dPipelineState.Get();
	}

	inline GPUGraphicsPipelineState::GPUGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept
		: d3dPipelineState(std::move(d3dPipelineState))
	{
	}
}
