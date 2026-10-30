#pragma once

#include <cassert>
#include <span>
#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
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
