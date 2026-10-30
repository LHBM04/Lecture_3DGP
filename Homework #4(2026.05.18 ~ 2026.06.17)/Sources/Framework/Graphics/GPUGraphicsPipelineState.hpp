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
