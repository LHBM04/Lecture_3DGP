#pragma once

#include "Pipeline.hpp"

namespace TUK::Framework
{
	class GraphicsDevice;

	class GraphicsPipeline final : public Pipeline
	{
		friend class GraphicsDevice;

	public:
		~GraphicsPipeline() noexcept override;

		GraphicsPipeline(const GraphicsPipeline&) = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
		GraphicsPipeline(GraphicsPipeline&& other) noexcept;
		GraphicsPipeline& operator=(GraphicsPipeline&& other) noexcept;

	private:
		GraphicsPipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
			ID3D12RootSignature& rootSignature) noexcept;
		void Bind(ID3D12GraphicsCommandList& commandList) const override;
	};
}
