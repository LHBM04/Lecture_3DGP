#pragma once

#include <expected>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class GPUGraphicsPipelineState
	{
	public:
		~GPUGraphicsPipelineState() noexcept = default;

		GPUGraphicsPipelineState(const GPUGraphicsPipelineState&) = delete;
		GPUGraphicsPipelineState& operator=(const GPUGraphicsPipelineState&) = delete;

		GPUGraphicsPipelineState(GPUGraphicsPipelineState&&) noexcept = default;
		GPUGraphicsPipelineState& operator=(GPUGraphicsPipelineState&&) noexcept = default;

		[[nodiscard]] ID3D12PipelineState& GetD3DPipelineState() const noexcept;

		[[nodiscard]] static std::expected<GPUGraphicsPipelineState, std::string> Create(
			const GPUDevice& device,
			const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description);

	private:
		explicit GPUGraphicsPipelineState(ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept;

		ComPtr<ID3D12PipelineState> d3dPipelineState;
	};

	inline ID3D12PipelineState& GPUGraphicsPipelineState::GetD3DPipelineState() const noexcept
	{
		return *d3dPipelineState.Get();
	}

	inline GPUGraphicsPipelineState::GPUGraphicsPipelineState(ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept
		: d3dPipelineState(std::move(d3dPipelineState))
	{
	}
}
