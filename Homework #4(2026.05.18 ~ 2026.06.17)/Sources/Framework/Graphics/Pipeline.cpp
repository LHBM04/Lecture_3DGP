#include "Precompiled.h"
#include "Pipeline.h"

#include <cassert>
#include <utility>

namespace TUK::Framework
{
	Pipeline::Pipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> state,
		ID3D12RootSignature& signature) noexcept
		: pipelineState(std::move(state))
		, rootSignature(&signature)
	{
		assert(pipelineState);
	}

	Pipeline::~Pipeline() noexcept = default;

	ID3D12PipelineState& Pipeline::GetPipelineState() const noexcept
	{
		assert(pipelineState);
		return *pipelineState.Get();
	}

	ID3D12RootSignature& Pipeline::GetRootSignature() const noexcept
	{
		assert(rootSignature);
		return *rootSignature.Get();
	}

	void Pipeline::Release() noexcept
	{
		pipelineState.Reset();
		rootSignature.Reset();
	}
}
