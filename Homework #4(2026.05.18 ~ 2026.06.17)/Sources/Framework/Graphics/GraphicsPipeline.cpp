#include "Precompiled.hpp"
#include "GraphicsPipeline.hpp"

namespace TUK::Framework
{
	GraphicsPipeline::GraphicsPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
		ID3D12RootSignature& rootSignature) noexcept
		: Pipeline(std::move(pipelineState), rootSignature)
	{
	}

	GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
		: Pipeline(std::move(other))
	{
	}

	GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
	{
		Pipeline::operator=(std::move(other));
		return *this;
	}

	GraphicsPipeline::~GraphicsPipeline() noexcept = default;

	void GraphicsPipeline::Bind(ID3D12GraphicsCommandList& commandList) const
	{
		commandList.SetPipelineState(&GetPipelineState());
		commandList.SetGraphicsRootSignature(&GetRootSignature());
	}
}
