#pragma once

#include "Pipeline.hpp"

namespace TUK::Framework
{
	class GraphicsPipeline : public Pipeline
	{
	public:
		virtual ~GraphicsPipeline() noexcept = default;

		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;

		GraphicsPipeline(GraphicsPipeline&&) noexcept = default;
		GraphicsPipeline& operator=(GraphicsPipeline&&) noexcept = default;

	protected:
		GraphicsPipeline() noexcept = default;
	};
}
