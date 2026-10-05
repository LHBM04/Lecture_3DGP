#include "Precompiled.h"
#include "ComputePipeline.h"

#include <utility>

namespace TUK::Framework
{
	ComputePipeline::ComputePipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
		ID3D12RootSignature& rootSignature) noexcept
		: Pipeline(std::move(pipelineState), rootSignature)
	{
	}

	ComputePipeline::~ComputePipeline() noexcept = default;

	void ComputePipeline::Bind(ID3D12GraphicsCommandList& commandList) const
	{
		commandList.SetPipelineState(&GetPipelineState());
		commandList.SetComputeRootSignature(&GetRootSignature());
	}
}
