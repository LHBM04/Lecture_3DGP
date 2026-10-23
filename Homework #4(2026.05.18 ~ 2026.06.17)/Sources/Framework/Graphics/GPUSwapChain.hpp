#pragma once

#include <cstdint>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl/client.h>

#include "GPUTarget.hpp"

namespace TUK::Framework
{
	class Display;

	struct GPUSwapChainProperties final
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

		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;
		[[nodiscard]] ID3D12Resource& GetD3DBackBuffer(UINT index) const;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle(UINT index) const;

	private:
		GPUSwapChain(
			Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
			std::vector<GPUTarget> renderTargets) noexcept;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
		std::vector<GPUTarget> renderTargets;
	};
}
