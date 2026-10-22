#pragma once

#include <functional>
#include <vector>

#include <d3d12.h>

namespace TUK::Framework
{
	struct RenderPassInfo;

	class GraphicsDevice;
	class RenderTarget;
	class Texture;
	class Shader;
	class Buffer;

	class CommandBuffer
	{
		friend class GraphicsDevice;

	public:
		CommandBuffer(ID3D12CommandAllocator* d3dCommandAllocator, ID3D12GraphicsCommandList4* d3dCommandList) noexcept;
		~CommandBuffer() noexcept;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&& other) noexcept;
		CommandBuffer& operator=(CommandBuffer&& other) noexcept;

		void BeginRenderPass(const RenderPassInfo& renderPassInfo);
		void EndRenderPass();

		void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation, UINT startInstanceLocation);

		/** D3D12 커맨드 할당자 Getter */
		[[nodiscard]] ID3D12CommandAllocator& GetD3D12CommandAllocator() const noexcept;

		/** D3D12 커맨드 리스트 Getter */
		[[nodiscard]] ID3D12GraphicsCommandList4& GetD3D12CommandList() const noexcept;

	private:
		ID3D12CommandAllocator* d3dCommandAllocator;
		ID3D12GraphicsCommandList4* d3dCommandList;
		std::vector<std::reference_wrapper<RenderTarget>> currentRenderTargets;
		D3D12_RESOURCE_STATES stateAfterRenderPass = D3D12_RESOURCE_STATE_PRESENT;
	};
}
