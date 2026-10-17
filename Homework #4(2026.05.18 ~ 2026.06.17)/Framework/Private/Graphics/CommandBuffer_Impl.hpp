#pragma once

#include <vector>

#include <d3d12.h>

#include "Framework/Graphics/CommandBuffer.hpp"

namespace TUK::Framework
{
	class CommandBuffer_Impl : public CommandBuffer
	{
	public:
		CommandBuffer_Impl(ID3D12CommandAllocator* d3dCommandAllocator, ID3D12GraphicsCommandList* d3dCommandList) noexcept;
		~CommandBuffer_Impl() noexcept override;

		void BeginFrame() override;
		void EndFrame() override;

		void SetRenderTarget(std::reference_wrapper<RenderTarget> renderTarget) override;
		void SetRenderTargets(std::span<std::reference_wrapper<RenderTarget>> renderTargets) override;

		void ClearRenderTarget(const ColorRGB<float>& clearColor) const override;
		void ClearRenderTarget(const ColorRGBA<float>& clearColor) const override;

		/** D3D12 커맨드 할당자 Getter */
		[[nodiscard]] ID3D12CommandAllocator& GetD3D12CommandAllocator() const noexcept;

		/** D3D12 커맨드 리스트 Getter */
		[[nodiscard]] ID3D12GraphicsCommandList& GetD3D12CommandList() const noexcept;

	private:
		ID3D12CommandAllocator* d3dCommandAllocator;
		ID3D12GraphicsCommandList* d3dCommandList;
		std::vector<std::reference_wrapper<RenderTarget>> currentRenderTargets;
	};
}
