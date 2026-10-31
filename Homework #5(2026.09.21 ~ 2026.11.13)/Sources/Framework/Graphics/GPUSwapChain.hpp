#pragma once

#include <memory_resource>
#include <utility>
#include <vector>

#include <wrl.h>

#include <directx/d3d12.h>

#include <dxgi1_6.h>

#include "../Core/Common.hpp"

#include "GPUTarget.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	struct GPUSwapChainProperties final
	{
		ComPtr<IDXGISwapChain4> dxgiSwapChain;
		ComPtr<ID3D12DescriptorHeap> d3dDescriptorHeap;
		std::pmr::vector<GPUTexture> backBuffers;
		u32 rtvDescriptorSize;
	};

	class GPUSwapChain
	{
	public:
		explicit GPUSwapChain(GPUSwapChainProperties&& properties) noexcept;
		~GPUSwapChain() noexcept = default;

		GPUSwapChain(const GPUSwapChain&) = delete;
		GPUSwapChain& operator=(const GPUSwapChain&) = delete;

		GPUSwapChain(GPUSwapChain&&) noexcept = default;
		GPUSwapChain& operator=(GPUSwapChain&&) noexcept = default;

		void Present() const;

		[[nodiscard]] GPUTarget GetCurrentBackBufferTarget() const noexcept;
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		ComPtr<IDXGISwapChain4> dxgiSwapChain;

		std::pmr::vector<GPUTexture> backBuffers;

		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		u32 rtvDescriptorSize;
	};

	inline GPUSwapChain::GPUSwapChain(GPUSwapChainProperties&& properties) noexcept
		: dxgiSwapChain(std::move(properties.dxgiSwapChain))
		, backBuffers(std::move(properties.backBuffers))
		, rtvHeap(std::move(properties.d3dDescriptorHeap))
		, rtvDescriptorSize(properties.rtvDescriptorSize)
	{
	}

	inline IDXGISwapChain4& GPUSwapChain::GetDXGISwapChain() const noexcept
	{
		return *dxgiSwapChain.Get();
	}
}
