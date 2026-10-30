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
	class D3DGraphicsPipelineState
	{
		friend class D3DDevice;

	public:
		~D3DGraphicsPipelineState() noexcept = default;

		D3DGraphicsPipelineState(const D3DGraphicsPipelineState&) = delete;
		D3DGraphicsPipelineState& operator=(const D3DGraphicsPipelineState&) = delete;

		D3DGraphicsPipelineState(D3DGraphicsPipelineState&&) noexcept = default;
		D3DGraphicsPipelineState& operator=(D3DGraphicsPipelineState&&) noexcept = default;

		[[nodiscard]] ID3D12PipelineState& GetD3DPipelineState() const noexcept;

	private:
		explicit D3DGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState;
	};

	inline ID3D12PipelineState& D3DGraphicsPipelineState::GetD3DPipelineState() const noexcept
	{
		assert(d3dPipelineState);
		return *d3dPipelineState.Get();
	}

	inline D3DGraphicsPipelineState::D3DGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12PipelineState> d3dPipelineState) noexcept
		: d3dPipelineState(std::move(d3dPipelineState))
	{
	}
}
