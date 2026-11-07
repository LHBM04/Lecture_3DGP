#pragma once

#include <expected>
#include <string>
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
	class Display;
	class GPUDevice;

	class GPUSwapChain
	{
	public:

		GPUSwapChain(const GPUSwapChain&) = delete;
		GPUSwapChain& operator=(const GPUSwapChain&) = delete;

		GPUSwapChain(GPUSwapChain&&) noexcept = default;
		GPUSwapChain& operator=(GPUSwapChain&&) noexcept = default;

		void Present() const;

		[[nodiscard]] GPUTarget GetCurrentBackBufferTarget() const noexcept;
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

		[[nodiscard]] static std::expected<GPUSwapChain, std::string> Create(
			const GPUDevice& device,
			const Display* display,
			const DXGI_SWAP_CHAIN_DESC1& description);

	private:
		GPUSwapChain(
			ComPtr<IDXGISwapChain4> dxgiSwapChain,
			ComPtr<ID3D12DescriptorHeap> rtvHeap,
			ComPtr<ID3D12DescriptorHeap> dsvHeap,
			ComPtr<ID3D12Resource> depthStencil,
			std::vector<GPUTexture2D>&& backBuffers,
			u32 rtvDescriptorSize) noexcept;

		ComPtr<IDXGISwapChain4> dxgiSwapChain;
		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		ComPtr<ID3D12DescriptorHeap> dsvHeap;
		ComPtr<ID3D12Resource> depthStencil;
		std::vector<GPUTexture2D> backBuffers;
		u32 rtvDescriptorSize;
	};

	inline IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *dxgiSwapChain.Get();
	}

	inline GPUSwapChain::GPUSwapChain(
		ComPtr<IDXGISwapChain4> dxgiSwapChain,
		ComPtr<ID3D12DescriptorHeap> rtvHeap,
		ComPtr<ID3D12DescriptorHeap> dsvHeap,
		ComPtr<ID3D12Resource> depthStencil,
		std::vector<GPUTexture2D>&& backBuffers,
		u32 rtvDescriptorSize) noexcept
		: dxgiSwapChain(std::move(dxgiSwapChain))
		, rtvHeap(std::move(rtvHeap))
		, dsvHeap(std::move(dsvHeap))
		, depthStencil(std::move(depthStencil))
		, backBuffers(std::move(backBuffers))
		, rtvDescriptorSize(rtvDescriptorSize)
	{
	}
}
