#pragma once

#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Framework/Graphics/SwapChain.hpp"

namespace TUK::Framework
{
	class SwapChain_Impl : public SwapChain
	{
	public:
		explicit SwapChain_Impl(IDXGISwapChain4* dxgiSwapChain) noexcept;
		~SwapChain_Impl() noexcept override;

		void Clear(CommandBuffer& commandBuffer) const override;
		void Present() const override;

		[[nodiscard]] Vector2D<int> GetSize() const noexcept override;
		void SetSize(const Vector2D<int>& size) noexcept override;

		/** DXGI ?ㅼ솑泥댁씤 Getter */
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		void CreateRenderTargets();
		void ReleaseRenderTargets() noexcept;

	private:
		IDXGISwapChain4* dxgiSwapChain;
		ID3D12DescriptorHeap* rtvHeap;
		std::vector<ID3D12Resource*> backBuffers;
		UINT rtvDescriptorSize;
	};
}
