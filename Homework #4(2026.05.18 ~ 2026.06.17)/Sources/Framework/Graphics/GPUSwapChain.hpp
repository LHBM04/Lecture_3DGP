#pragma once

#include <dxgi1_6.h>

namespace TUK::Framework
{
	class GPUSwapChain
	{
	public:
		~GPUSwapChain() noexcept;

		GPUSwapChain(const GPUSwapChain&) = delete;
		GPUSwapChain& operator=(const GPUSwapChain&) = delete;

		GPUSwapChain(GPUSwapChain&&) noexcept = delete;
		GPUSwapChain& operator=(GPUSwapChain&&) noexcept = delete;

		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		explicit GPUSwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept;

		IDXGISwapChain4* dxgiSwapChain;
	};
}
