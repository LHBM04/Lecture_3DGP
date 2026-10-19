#pragma once

#include "Pipeline.hpp"

namespace TUK::Framework
{
	class ComputePipeline : public Pipeline
	{
	public:
		virtual ~ComputePipeline() noexcept = default;

	protected:
		ComputePipeline() noexcept = default;
	};
}
