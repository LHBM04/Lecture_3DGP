#include "Precompiled.hpp"
#include "GPUGraphicsPipelineState.hpp"

#include <format>
#include <iterator>

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/ShaderAsset.hpp"

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	std::expected<GPUGraphicsPipelineState, std::string> GPUGraphicsPipelineState::Create(
		const GPUDevice& device,
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription = description;
		pipelineDescription.pRootSignature = &device.GetD3DRootSignature();

		ComPtr<ID3D12PipelineState> d3dPipelineState;
		const HRESULT result = device.GetD3DDevice().CreateGraphicsPipelineState(
			&pipelineDescription,
			IID_PPV_ARGS(d3dPipelineState.GetAddressOf()));
		if (FAILED(result))
		{
			return std::unexpected(std::format(
				"GPUGraphicsPipelineState 생성에 실패했습니다. HRESULT: 0x{:08X}",
				static_cast<u32>(result)));
		}

		return GPUGraphicsPipelineState(std::move(d3dPipelineState));
	}

	std::expected<GPUGraphicsPipelineState, std::string> GPUGraphicsPipelineState::CreateForMaterial(
		const GPUDevice& device,
		const MaterialAsset& material,
		const ShaderAsset& vertexShader,
		const ShaderAsset& pixelShader,
		DXGI_FORMAT targetFormat)
	{
		static constexpr D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.VS = vertexShader.GetShader().GetD3DBytecode();
		description.PS = pixelShader.GetShader().GetD3DBytecode();
		description.InputLayout = { INPUT_ELEMENTS, static_cast<UINT>(std::size(INPUT_ELEMENTS)) };
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = targetFormat;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.FillMode = material.GetFillMode();
		description.RasterizerState.CullMode = material.GetCullMode();
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DepthStencilState.DepthEnable = FALSE;
		return Create(device, description);
	}

}
