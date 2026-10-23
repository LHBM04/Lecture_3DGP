#pragma once

#include <span>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUCommandBuffer
	{
		friend class GPUDevice;

	public:
		~GPUCommandBuffer() noexcept;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		// 리소스 상태 전환은 호출자가 패스 시작 전과 종료 후에 처리합니다.
		void BeginRenderPass(
			std::span<const D3D12_RENDER_PASS_RENDER_TARGET_DESC> renderTargets,
			const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC* depthStencil = nullptr);
		void EndRenderPass();

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		explicit GPUCommandBuffer(
			ID3D12GraphicsCommandList10* d3dCommandList,
			ID3D12CommandAllocator* d3dCommandAllocator) noexcept;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		bool isRenderPassActive = false;
	};
}
