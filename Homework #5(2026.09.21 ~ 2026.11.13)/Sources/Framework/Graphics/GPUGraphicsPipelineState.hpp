#pragma once

#include <wrl/client.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUGraphicsPipelineState
	{
	public:
		explicit GPUGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept;

		GPUGraphicsPipelineState(const GPUGraphicsPipelineState&) = delete;
		GPUGraphicsPipelineState& operator=(const GPUGraphicsPipelineState&) = delete;

		GPUGraphicsPipelineState(GPUGraphicsPipelineState&&) noexcept = default;
		GPUGraphicsPipelineState& operator=(GPUGraphicsPipelineState&&) noexcept = default;

		[[nodiscard]] ID3D12PipelineState& GetD3DPipelineState() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
	};

	inline GPUGraphicsPipelineState::GPUGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept
		: d3dPipelineState(std::move(d3dPipelineState))
	{
		ASSERT_FMT(this->d3dPipelineState.Get(), "d3dPipelineState가 nullptr입니다.");
	}

	inline ID3D12PipelineState& GPUGraphicsPipelineState::GetD3DPipelineState() const noexcept
	{
		ASSERT_FMT(d3dPipelineState.Get(), "d3dPipelineState가 nullptr입니다.");
		return *d3dPipelineState.Get();
	}
}
