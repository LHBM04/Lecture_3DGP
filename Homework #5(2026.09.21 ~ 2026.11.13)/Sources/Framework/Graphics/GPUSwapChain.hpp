#pragma once

#include <utility>
#include <vector>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <dxgi1_6.h>

#include "../Core/Common.hpp"

#include "GPUTarget.hpp"
#include "GPUTexture2D.hpp"

namespace TUK::Framework
{
	struct GPUSwapChainCreateInfo final
	{
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
		std::vector<GPUTexture2D> backBuffers;
		u32 rtvDescriptorSize = 0;
	};

	class GPUSwapChain
	{
	public:
		explicit GPUSwapChain(GPUSwapChainCreateInfo&& createInfo) noexcept;

		GPUSwapChain(const GPUSwapChain&) = delete;
		GPUSwapChain& operator=(const GPUSwapChain&) = delete;

		GPUSwapChain(GPUSwapChain&&) noexcept = default;
		GPUSwapChain& operator=(GPUSwapChain&&) noexcept = default;

		void Present() const;

		[[nodiscard]] GPUTarget GetCurrentBackBufferTarget() const noexcept;
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		UINT rtvDescriptorSize;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
		
		std::vector<GPUTexture2D> backBuffers;
	};

	inline GPUSwapChain::GPUSwapChain(GPUSwapChainCreateInfo&& createInfo) noexcept
		: dxgiSwapChain(std::move(createInfo.dxgiSwapChain))
		, rtvHeap(std::move(createInfo.rtvHeap))
		, dsvHeap(std::move(createInfo.dsvHeap))
		, depthStencil(std::move(createInfo.depthStencil))
		, backBuffers(std::move(createInfo.backBuffers))
		, rtvDescriptorSize(createInfo.rtvDescriptorSize)
	{
		ASSERT_FMT(this->dxgiSwapChain.Get(), "dxgiSwapChain이 nullptr입니다.");
		ASSERT_FMT(this->rtvHeap.Get(), "rtvHeap이 nullptr입니다.");
		ASSERT_FMT(this->dsvHeap.Get(), "dsvHeap이 nullptr입니다.");
		ASSERT_FMT(this->depthStencil.Get(), "depthStencil이 nullptr입니다.");
	}

	inline IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		ASSERT_FMT(dxgiSwapChain.Get(), "dxgiSwapChain이 nullptr입니다.");
		return *dxgiSwapChain.Get();
	}
}
