#pragma once

#include <cstdint>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl/client.h>

#include "GPUTarget.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	class Display;

	struct GPUSwapChainCreateInfo final
	{
		const Display* display;
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t bufferCount;
	};

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

		[[nodiscard]] GPUTarget GetCurrentBackBufferTarget() const noexcept;
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
