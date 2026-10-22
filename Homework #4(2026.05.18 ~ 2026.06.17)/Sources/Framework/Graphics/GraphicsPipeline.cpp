#include "Precompiled.hpp"
#include "GraphicsPipeline.hpp"

namespace TUK::Framework
{
	GraphicsPipeline::GraphicsPipeline(ID3D12PipelineState* pipelineState) noexcept
		: Pipeline(pipelineState)
	{
		
	}
}
