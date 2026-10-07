#include "Precompiled.h"
#include "Pipeline.h"

namespace TUK::Framework
{
	Pipeline::Pipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> state,
		ID3D12RootSignature& signature) noexcept
		: pipelineState(std::move(state))
		, rootSignature(&signature)
	{
		assert(pipelineState);
	}

	Pipeline::Pipeline(Pipeline&& other) noexcept
		: pipelineState(std::move(other.pipelineState))
		, rootSignature(std::move(other.rootSignature))
	{
	}

	Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
	{
		if (this != &other)
		{
			pipelineState = std::move(other.pipelineState);
			rootSignature = std::move(other.rootSignature);
		}
		return *this;
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
