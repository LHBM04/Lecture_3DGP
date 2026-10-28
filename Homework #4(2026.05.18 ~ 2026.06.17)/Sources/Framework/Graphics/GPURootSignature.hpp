#pragma once

#include <cassert>
#include <span>
#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	struct GPURootSignatureCreateInfo final
	{
		// Root Signature 1.0을 사용합니다. 내부 descriptor range 배열도 생성 호출까지 유효해야 합니다.
		std::span<const D3D12_ROOT_PARAMETER> parameters;
		std::span<const D3D12_STATIC_SAMPLER_DESC> staticSamplers;
		D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	};

	class GPURootSignature
	{
		friend class GPUDevice;

	public:
		~GPURootSignature() noexcept = default;

		GPURootSignature(const GPURootSignature&) = delete;
		GPURootSignature& operator=(const GPURootSignature&) = delete;

		GPURootSignature(GPURootSignature&&) noexcept = default;
		GPURootSignature& operator=(GPURootSignature&&) noexcept = default;

		[[nodiscard]] ID3D12RootSignature& GetD3DRootSignature() const noexcept;

	private:
		explicit GPURootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	inline ID3D12RootSignature& GPURootSignature::GetD3DRootSignature() const noexcept
	{
		assert(d3dRootSignature);
		return *d3dRootSignature.Get();
	}

	inline GPURootSignature::GPURootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept
		: d3dRootSignature(std::move(d3dRootSignature))
	{
	}
}
