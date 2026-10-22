#pragma once

namespace TUK::Framework
{
	class Pipeline
	{
	public:
		virtual ~Pipeline() noexcept
		{
			if (d3dPipelineState)
			{
				d3dPipelineState->Release();
				d3dPipelineState = nullptr;
			}
		}

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		Pipeline(Pipeline&& other) noexcept
			: d3dPipelineState(std::exchange(other.d3dPipelineState, nullptr))
		{
		}

		Pipeline& operator=(Pipeline&& other) noexcept
		{
			if (this != &other)
			{
				if (d3dPipelineState)
				{
					d3dPipelineState->Release();
					d3dPipelineState = nullptr;
				}
				d3dPipelineState = std::exchange(other.d3dPipelineState, nullptr);
			}
			return *this;
		}

	protected:
		Pipeline(ID3D12PipelineState* pipelineState) noexcept
			: d3dPipelineState(pipelineState)
		{
		}

	private:
		ID3D12PipelineState* d3dPipelineState;
	};
}
