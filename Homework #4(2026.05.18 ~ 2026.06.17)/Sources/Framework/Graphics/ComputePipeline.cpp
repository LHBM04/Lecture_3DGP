#include "Precompiled.h"
#include "ComputePipeline.h"

namespace TUK::Framework
{
	ComputePipeline::ComputePipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
		ID3D12RootSignature& rootSignature) noexcept
		: Pipeline(std::move(pipelineState), rootSignature)
	{
	}

	ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
		: Pipeline(std::move(other))
	{
	}

	ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
	{
		Pipeline::operator=(std::move(other));
		return *this;
	}

	ComputePipeline::~ComputePipeline() noexcept = default;

	void ComputePipeline::Bind(ID3D12GraphicsCommandList& commandList) const
	{
		commandList.SetPipelineState(&GetPipelineState());
		commandList.SetComputeRootSignature(&GetRootSignature());
	}
}
