#pragma once

#include <cstdint>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl/client.h>

#include "D3DTarget.hpp"
#include "D3DTexture.hpp"

namespace TUK::Framework
{
	class D3DSwapChain
	{
		friend class RenderSubsystem;

	public:
		~D3DSwapChain() noexcept = default;

		D3DSwapChain(const D3DSwapChain&) = delete;
		D3DSwapChain& operator=(const D3DSwapChain&) = delete;

		D3DSwapChain(D3DSwapChain&&) noexcept = default;
		D3DSwapChain& operator=(D3DSwapChain&&) noexcept = default;

		void Present() const;

		[[nodiscard]] D3DTarget GetCurrentBackBufferTarget() const noexcept;
		[[nodiscard]] IDXGISwapChain4& GetDXGISwapChain() const noexcept;

	private:
		explicit D3DSwapChain(
			Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain,
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap,
			std::vector<D3DTexture> backBuffers,
			std::uint32_t rtvDescriptorSize) noexcept;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> dxgiSwapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;

		std::vector<D3DTexture> backBuffers;
		std::uint32_t rtvDescriptorSize;
	};
}
