#include "Precompiled.hpp"
#include "MaterialAsset.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	std::expected<MaterialAsset, std::string> MaterialAsset::Create(
		const GPUDevice& device,
		const ShaderAsset& vertexShader, const ShaderAsset& pixelShader,
		DXGI_FORMAT targetFormat, D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
	{
		const std::array<D3D12_INPUT_ELEMENT_DESC, 3> inputElements
		{
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(Vertex, position)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "NORMAL",
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(Vertex, normal)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "COLOR",
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(Vertex, color)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			},
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineInfo{};
		pipelineInfo.VS = vertexShader.GetShader().GetD3DBytecode();
		pipelineInfo.PS = pixelShader.GetShader().GetD3DBytecode();
		pipelineInfo.InputLayout = { inputElements.data(), static_cast<UINT>(inputElements.size()) };
		pipelineInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineInfo.NumRenderTargets = 1;
		pipelineInfo.RTVFormats[0] = targetFormat;
		pipelineInfo.SampleMask = UINT_MAX;
		pipelineInfo.SampleDesc = { 1, 0 };
		pipelineInfo.RasterizerState.FillMode = fillMode;
		pipelineInfo.RasterizerState.CullMode = cullMode;
		pipelineInfo.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		pipelineInfo.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipelineInfo.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipelineInfo.RasterizerState.DepthClipEnable = TRUE;
		for (D3D12_RENDER_TARGET_BLEND_DESC& target : pipelineInfo.BlendState.RenderTarget)
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
		pipelineInfo.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipelineInfo.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineInfo.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		pipelineInfo.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		pipelineInfo.DepthStencilState.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		pipelineInfo.DepthStencilState.BackFace = pipelineInfo.DepthStencilState.FrontFace;
		auto pipeline = device.CreateGPUGraphicsPipelineState(pipelineInfo);
		if (!pipeline)
		{
			return std::unexpected(pipeline.error());
		}
		return MaterialAsset(std::move(*pipeline), targetFormat, cullMode, fillMode);
	}
}
