#pragma once

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class GPUTarget
	{
	public:
		GPUTarget(
			Microsoft::WRL::ComPtr<ID3D12Resource2> d3dResource,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle) noexcept;
		~GPUTarget() noexcept = default;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) noexcept = default;
		GPUTarget& operator=(GPUTarget&&) noexcept = default;

		[[nodiscard]] ID3D12Resource& GetD3DResource() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource2> d3dResource;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	};
}
