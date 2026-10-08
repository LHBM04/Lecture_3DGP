#pragma once

#include <d3d12.h>

#include <wrl.h>

namespace TUK::Framework
{
	class RenderContext;

	class Pipeline
	{
		friend class RenderContext;

	public:
		virtual ~Pipeline() noexcept;

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		/** GPU가 사용을 마친 뒤 호출한다. */
		void Release() noexcept;

	protected:
		Pipeline(Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState,
			ID3D12RootSignature& rootSignature) noexcept;
		Pipeline(Pipeline&& other) noexcept;
		Pipeline& operator=(Pipeline&& other) noexcept;

		[[nodiscard]] ID3D12PipelineState& GetPipelineState() const noexcept;
		[[nodiscard]] ID3D12RootSignature& GetRootSignature() const noexcept;

	private:
		virtual void Bind(ID3D12GraphicsCommandList& commandList) const = 0;

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	};
}
