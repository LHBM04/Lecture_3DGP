#pragma once

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUTexture2D;

	struct GPUTargetCreateInfo final
	{
		const GPUTexture2D* texture = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	};

	class GPUTarget
	{
	public:
		static constexpr DXGI_FORMAT DEPTH_FORMAT = DXGI_FORMAT_D32_FLOAT;
		static constexpr float DEPTH_CLEAR_VALUE = 1.0f;

		explicit GPUTarget(const GPUTargetCreateInfo& createInfo) noexcept;

		GPUTarget(const GPUTarget&) = delete;
		GPUTarget& operator=(const GPUTarget&) = delete;

		GPUTarget(GPUTarget&&) = delete;
		GPUTarget& operator=(GPUTarget&&) = delete;

		[[nodiscard]] const GPUTexture2D& GetTexture() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DRTVHandle() const noexcept;
		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetD3DDSVHandle() const noexcept;
		[[nodiscard]] DXGI_FORMAT GetDXGIFormat() const noexcept;

	private:
		const GPUTexture2D* texture;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
		DXGI_FORMAT format;
	};

	inline GPUTarget::GPUTarget(const GPUTargetCreateInfo& createInfo) noexcept
		: texture(createInfo.texture)
		, rtvHandle(createInfo.rtvHandle)
		, dsvHandle(createInfo.dsvHandle)
		, format(createInfo.format)
	{
		ASSERT_FMT(this->texture, "texture가 nullptr입니다.");
	}

	inline const GPUTexture2D& GPUTarget::GetTexture() const noexcept
	{
		return *texture;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DRTVHandle() const noexcept
	{
		return rtvHandle;
	}

	inline D3D12_CPU_DESCRIPTOR_HANDLE GPUTarget::GetD3DDSVHandle() const noexcept
	{
		return dsvHandle;
	}

	inline DXGI_FORMAT GPUTarget::GetDXGIFormat() const noexcept
	{
		return format;
	}
}
