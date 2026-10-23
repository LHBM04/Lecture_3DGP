#pragma once

#include <dxgi1_6.h>

#include <wrl/client.h>

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

		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		explicit GPUSwapChain(IDXGISwapChain4* dxgiSwapChain) noexcept;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;
	};
}
