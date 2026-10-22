#pragma once

#include "Pipeline.hpp"

namespace TUK::Framework
{
	class GraphicsPipeline : public Pipeline
	{
	public:
		GraphicsPipeline(ID3D12PipelineState* pipelineState) noexcept;
	};
}
