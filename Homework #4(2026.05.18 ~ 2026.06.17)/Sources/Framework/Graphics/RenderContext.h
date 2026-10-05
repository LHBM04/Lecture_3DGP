#pragma once

#include <d3d12.h>
#include <array>
#include <cstdint>
#include <expected>
#include <functional>
#include <span>
#include <string>
#include <wrl.h>

namespace TUK::Framework
{
	class SwapChain;
	class Pipeline;
	class Buffer;
	class RenderSubsystem;

	class RenderContext
	{
		friend class RenderSubsystem;

	public:
		RenderContext() noexcept;
		~RenderContext() noexcept = default;

		RenderContext(const RenderContext&) = delete;
		RenderContext& operator=(const RenderContext&) = delete;

		RenderContext(RenderContext&&) = delete;
		RenderContext& operator=(RenderContext&&) = delete;

		[[nodiscard]] std::expected<void, std::string> Initialize(ID3D12Device& device);
		/** 이전 명령의 GPU 실행이 완료된 뒤 호출. */
		[[nodiscard]] std::expected<void, std::string> Begin();
		[[nodiscard]] std::expected<void, std::string> End();
		void Release() noexcept;

		[[nodiscard]] std::expected<void, std::string> SetSwapChain(SwapChain& target);
		[[nodiscard]] std::expected<void, std::string> ClearSwapChain(SwapChain& target, const std::array<float, 4>& color);
		[[nodiscard]] std::expected<void, std::string> EndSwapChain(SwapChain& target);

		/** Begin과 End 사이에서 호출하는 명령 기록 인터페이스. */
		void SetPipeline(const Pipeline& pipeline);
		void SetDescriptorHeaps(std::span<const std::reference_wrapper<ID3D12DescriptorHeap>> heaps);
		void SetRootDescriptorTable(UINT parameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptor);
		void SetRootConstantBufferView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
		void SetRootShaderResourceView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
		void SetRoot32BitConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset);
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
		void SetVertexBuffers(UINT startSlot, std::span<const D3D12_VERTEX_BUFFER_VIEW> views);
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view);
		void SetViewports(std::span<const D3D12_VIEWPORT> viewports);
		void SetScissorRects(std::span<const D3D12_RECT> rectangles);
		void DrawInstanced(UINT vertexCount, UINT instanceCount, UINT startVertex, UINT startInstance);
		void DrawIndexedInstanced(UINT indexCount, UINT instanceCount, UINT startIndex, INT baseVertex, UINT startInstance);
		void SetComputeRootDescriptorTable(UINT parameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE descriptor);
		void SetComputeRootConstantBufferView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
		void SetComputeRootShaderResourceView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
		void SetComputeRootUnorderedAccessView(UINT parameterIndex, D3D12_GPU_VIRTUAL_ADDRESS address);
		void SetComputeRoot32BitConstants(UINT parameterIndex, std::span<const std::uint32_t> values, UINT offset);
		void Dispatch(UINT groupCountX, UINT groupCountY, UINT groupCountZ);
		void ResourceBarriers(std::span<const D3D12_RESOURCE_BARRIER> barriers);
		/** 상태 전환은 호출자가 지정하며 자동으로 추적하지 않는다. */
		void TransitionBuffer(const Buffer& buffer, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
		/** 서로 다른 버퍼를 복사한다. COPY_SOURCE/DEST 상태와 GPU 완료 전 수명은 호출자가 보장한다. */
		[[nodiscard]] std::expected<void, std::string> CopyBuffer(const Buffer& destination, UINT64 destinationOffset,
			const Buffer& source, UINT64 sourceOffset, UINT64 size);
		void CopyResource(ID3D12Resource& destination, ID3D12Resource& source);
		[[nodiscard]] bool IsRecording() const noexcept;

	private:
		void AssertInitialized() const noexcept;
		[[nodiscard]] std::expected<void, std::string> Execute(ID3D12CommandQueue& queue);

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		bool isRecording;
	};
}
