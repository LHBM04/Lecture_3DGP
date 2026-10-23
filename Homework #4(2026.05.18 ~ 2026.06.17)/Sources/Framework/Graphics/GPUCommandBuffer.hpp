#pragma once

#include <array>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUSwapChain;

	class GPUCommandBuffer
	{
		friend class GPUDevice;

	public:
		~GPUCommandBuffer() noexcept = default;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		// 명령 목록은 기록 중이어야 하며, 패스 도중 Present/ResizeBuffers를 호출하지 않습니다.
		void BeginRenderPass(const GPUSwapChain& swapChain);
		void EndRenderPass();

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		explicit GPUCommandBuffer(
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Resource> activeRenderTarget;
	};
}
