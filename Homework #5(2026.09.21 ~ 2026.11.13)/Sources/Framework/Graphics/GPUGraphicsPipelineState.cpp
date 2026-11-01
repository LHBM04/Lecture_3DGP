#include "Precompiled.hpp"
#include "GPUGraphicsPipelineState.hpp"

#include <format>

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
}
