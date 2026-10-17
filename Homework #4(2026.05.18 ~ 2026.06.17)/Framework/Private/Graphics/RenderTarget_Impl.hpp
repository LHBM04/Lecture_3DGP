#pragma once

#include <d3d12.h>

#include "Framework/Graphics/RenderTarget.hpp"

namespace TUK::Framework
{
	class RenderTarget_Impl : public RenderTarget
	{
	public:
		explicit RenderTarget_Impl(ID3D12Resource* d3dResource) noexcept;
		~RenderTarget_Impl() noexcept override;

		/** D3D12 리소스 Getter */
		[[nodiscard]] ID3D12Resource& GetD3D12Resource() const noexcept;

		/** D3D12 렌더 타겟 뷰 핸들 Getter */
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3D12RTVHandle() const noexcept;

	private:
		ID3D12Resource* d3dResource;
		ID3D12DescriptorHeap* d3dRtvHeap;
	};
}