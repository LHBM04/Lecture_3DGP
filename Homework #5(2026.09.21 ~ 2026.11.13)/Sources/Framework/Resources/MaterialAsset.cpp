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
				.AlignedByteOffset = static_cast<u32>(offsetof(Vertex, position)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "NORMAL",
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.AlignedByteOffset = static_cast<u32>(offsetof(Vertex, normal)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "COLOR",
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.AlignedByteOffset = static_cast<u32>(offsetof(Vertex, color)),
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
		pipelineInfo.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		pipelineInfo.RasterizerState.FillMode = fillMode;
		pipelineInfo.RasterizerState.CullMode = cullMode;
		pipelineInfo.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		pipelineInfo.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		pipelineInfo.DepthStencilState.DepthEnable = FALSE;
		auto pipeline = device.CreateGPUGraphicsPipelineState(pipelineInfo);
		if (!pipeline)
		{
			return std::unexpected(pipeline.error());
		}
		return MaterialAsset(std::move(*pipeline), targetFormat, cullMode, fillMode);
	}
}
