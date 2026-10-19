#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class CommandBuffer;

	class RenderTarget
	{
		friend class CommandBuffer;

	public:
		explicit RenderTarget(ID3D12Resource* d3dResource) noexcept;
		~RenderTarget() noexcept;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&) = delete;
		RenderTarget& operator=(RenderTarget&&) = delete;

		/** D3D12 리소스 Getter */
		[[nodiscard]] ID3D12Resource& GetD3D12Resource() const noexcept;

		/** D3D12 렌더 타겟 뷰 핸들 Getter */
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3D12RTVHandle() const noexcept;

	private:
		ID3D12Resource* d3dResource;
		ID3D12DescriptorHeap* d3dRtvHeap;
	};
}
