#pragma once

#include "Pipeline.hpp"

namespace TUK::Framework
{
	class GraphicsDevice;

	class ComputePipeline final : public Pipeline
	{
		friend class GraphicsDevice;

	public:
		~ComputePipeline() noexcept override;

		ComputePipeline(const ComputePipeline&) = delete;
		ComputePipeline& operator=(const ComputePipeline&) = delete;
		ComputePipeline(ComputePipeline&& other) noexcept;
		ComputePipeline& operator=(ComputePipeline&& other) noexcept;

	private:
		ComputePipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
			ID3D12RootSignature& rootSignature) noexcept;
		void Bind(ID3D12GraphicsCommandList& commandList) const override;
	};
}
