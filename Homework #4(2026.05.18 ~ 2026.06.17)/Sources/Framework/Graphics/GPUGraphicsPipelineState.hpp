#pragma once

#include <utility>

#include <directx/d3d12.h>

#include <wrl/client.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUGraphicsPipelineState
	{
	public:
		explicit GPUGraphicsPipelineState(ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept;
		~GPUGraphicsPipelineState() noexcept = default;

		GPUGraphicsPipelineState(const GPUGraphicsPipelineState&) = delete;
		GPUGraphicsPipelineState& operator=(const GPUGraphicsPipelineState&) = delete;

		GPUGraphicsPipelineState(GPUGraphicsPipelineState&&) noexcept = default;
		GPUGraphicsPipelineState& operator=(GPUGraphicsPipelineState&&) noexcept = default;

		[[nodiscard]] ID3D12PipelineState& GetD3DPipelineState() const noexcept;

	private:
		ComPtr<ID3D12PipelineState> d3dPipelineState;
	};

	inline GPUGraphicsPipelineState::GPUGraphicsPipelineState(ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept
		: d3dPipelineState(std::move(d3dPipelineState))
	{
	}

	inline ID3D12PipelineState& GPUGraphicsPipelineState::GetD3DPipelineState() const noexcept
	{
		return *d3dPipelineState.Get();
	}
}
