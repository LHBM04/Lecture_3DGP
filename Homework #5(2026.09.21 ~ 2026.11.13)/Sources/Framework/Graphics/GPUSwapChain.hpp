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

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class Display;
	class GPUDevice;

	class GPUSwapChain
	{
	public:
		explicit GPUSwapChain(
			Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap,
			Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil,
			std::vector<GPUTexture2D>&& backBuffers,
			u32 rtvDescriptorSize) noexcept;

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
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		UINT rtvDescriptorSize;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil;
		
		std::vector<GPUTexture2D> backBuffers;
	};

	inline GPUSwapChain::GPUSwapChain(
		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap,
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencil,
		std::vector<GPUTexture2D>&& backBuffers,
		u32 rtvDescriptorSize) noexcept
		: dxgiSwapChain(std::move(dxgiSwapChain))
		, rtvHeap(std::move(rtvHeap))
		, dsvHeap(std::move(dsvHeap))
		, depthStencil(std::move(depthStencil))
		, backBuffers(std::move(backBuffers))
		, rtvDescriptorSize(rtvDescriptorSize)
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
