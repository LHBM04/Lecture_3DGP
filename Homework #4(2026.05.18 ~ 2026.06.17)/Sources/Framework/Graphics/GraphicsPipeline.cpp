#include "Precompiled.h"
#include "GraphicsPipeline.h"

namespace TUK::Framework
{
	GraphicsPipeline::GraphicsPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
		ID3D12RootSignature& rootSignature) noexcept
		: Pipeline(std::move(pipelineState), rootSignature)
	{
	}

	GraphicsPipeline::~GraphicsPipeline() noexcept
	{
	}

	void GraphicsPipeline::Bind(ID3D12GraphicsCommandList& commandList) const
	{
		commandList.SetPipelineState(&GetPipelineState());
		commandList.SetGraphicsRootSignature(&GetRootSignature());
	}
}
