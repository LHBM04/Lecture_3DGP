#pragma once

#include <cassert>
#include <span>
#include <utility>

#include <d3d12.h>

#include <wrl/client.h>

namespace TUK::Framework
{
	class D3DRootSignature
	{
		friend class D3DDevice;

	public:
		~D3DRootSignature() noexcept = default;

		D3DRootSignature(const D3DRootSignature&) = delete;
		D3DRootSignature& operator=(const D3DRootSignature&) = delete;

		D3DRootSignature(D3DRootSignature&&) noexcept = default;
		D3DRootSignature& operator=(D3DRootSignature&&) noexcept = default;

		[[nodiscard]] ID3D12RootSignature& GetD3DRootSignature() const noexcept;

	private:
		explicit D3DRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept;

		Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature;
	};

	inline ID3D12RootSignature& D3DRootSignature::GetD3DRootSignature() const noexcept
	{
		assert(d3dRootSignature);
		return *d3dRootSignature.Get();
	}

	inline D3DRootSignature::D3DRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> d3dRootSignature) noexcept
		: d3dRootSignature(std::move(d3dRootSignature))
	{
	}
}
