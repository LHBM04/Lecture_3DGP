#include "Precompiled.hpp"
#include "MaterialAsset.hpp"

#include <cstdint>
#include <limits>
#include <utility>

#include <directxtk12/VertexTypes.h>

#include "../Graphics/GPUDevice.hpp"

#include "ShaderAsset.hpp"

namespace TUK::Framework
{
	std::expected<MaterialAsset, std::string> MaterialAsset::Create(
		const GPUDevice& device,
		const ShaderAsset& vertexShader, const ShaderAsset& pixelShader,
		DXGI_FORMAT targetFormat, D3D12_CULL_MODE cullMode, D3D12_FILL_MODE fillMode)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineInfo{};
		pipelineInfo.VS = vertexShader.GetShader().GetD3DBytecode();
		pipelineInfo.PS = pixelShader.GetShader().GetD3DBytecode();
		pipelineInfo.InputLayout = DirectX::VertexPositionNormalColor::InputLayout;
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
