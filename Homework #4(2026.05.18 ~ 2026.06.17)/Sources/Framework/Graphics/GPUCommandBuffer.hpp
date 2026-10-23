#pragma once

#include <d3d12.h>

#include <wrl/client.h>

#include "../Math/ColorRGBA.hpp"

namespace TUK::Framework
{
	class GPUTarget;
	class GPUTexture;

	class GPUCommandBuffer
	{
		friend class GPUDevice;

	public:
		~GPUCommandBuffer() noexcept = default;

		GPUCommandBuffer(const GPUCommandBuffer&) = delete;
		GPUCommandBuffer& operator=(const GPUCommandBuffer&) = delete;

		GPUCommandBuffer(GPUCommandBuffer&&) noexcept = default;
		GPUCommandBuffer& operator=(GPUCommandBuffer&&) noexcept = default;

		// 명령 목록은 기록 중이어야 하며, 호출자가 현재 리소스 상태를 지정합니다.
		void TransitionTexture(
			const GPUTexture& texture,
			D3D12_RESOURCE_STATES stateBefore,
			D3D12_RESOURCE_STATES stateAfter);

		// 타겟을 Clear하거나 그릴 때는 RENDER_TARGET 상태여야 합니다.
		void ClearGPUTarget(const GPUTarget& target, const ColorRGBA<float>& clearColor);
		void SetGPUTarget(const GPUTarget& target);

		[[nodiscard]] ID3D12GraphicsCommandList10& GetD3DCommandList() const noexcept;
		[[nodiscard]] ID3D12CommandAllocator& GetD3DCommandAllocator() const noexcept;

	private:
		explicit GPUCommandBuffer(
			Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList,
			Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator) noexcept;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
	};
}
