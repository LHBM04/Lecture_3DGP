#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class GPUTexture;

	class GPUTarget
	{
		friend class GPUSwapChain;

	public:
		~GPUTarget() noexcept = default;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) noexcept = default;
		GPUTarget& operator=(GPUTarget&&) noexcept = default;

		[[nodiscard]] const GPUTexture& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		explicit GPUTarget(
			const GPUTexture& texture,
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle,
			DXGI_FORMAT format) noexcept;

		const GPUTexture* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		DXGI_FORMAT format;
	};
}
