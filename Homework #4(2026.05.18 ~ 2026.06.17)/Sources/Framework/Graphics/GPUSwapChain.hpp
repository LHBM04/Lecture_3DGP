#pragma once

#include <cstdint>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl/client.h>

#include "GPURenderTarget.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	class GPUSwapChain
	{
		friend class RenderSubsystem;

	public:
		~GPUSwapChain() noexcept = default;

		GPUSwapChain(const GPUSwapChain&) = delete;
		GPUSwapChain& operator=(const GPUSwapChain&) = delete;

		GPUSwapChain(GPUSwapChain&&) noexcept = default;
		GPUSwapChain& operator=(GPUSwapChain&&) noexcept = default;

		void Present() const;

		[[nodiscard]] GPURenderTarget GetCurrentBackBufferTarget() const noexcept;
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		explicit GPUSwapChain(
			Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
			std::vector<GPUTexture> backBuffers,
			std::uint32_t rtvDescriptorSize) noexcept;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;

		std::vector<GPUTexture> backBuffers;
		std::uint32_t rtvDescriptorSize;
	};
}
