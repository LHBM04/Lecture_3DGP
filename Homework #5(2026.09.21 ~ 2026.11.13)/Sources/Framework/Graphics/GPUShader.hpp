#pragma once

#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <directx-dxc/dxcapi.h>

#include "../Core/Common.hpp"

#include "../Utilities/Logger.hpp"

namespace TUK::Framework
{
	class GPUShader
	{
	public:
		explicit GPUShader(Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob) noexcept;

		GPUShader(const GPUShader&) = delete;
		GPUShader& operator=(const GPUShader&) = delete;

		GPUShader(GPUShader&&) noexcept = default;
		GPUShader& operator=(GPUShader&&) noexcept = default;

		[[nodiscard]] IDxcBlob& GetDXCBlob() const noexcept;
		[[nodiscard]] D3D12_SHADER_BYTECODE GetD3DBytecode() const noexcept;

	private:
		Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob;
	};

	inline GPUShader::GPUShader(Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob) noexcept
		: dxcBlob(std::move(dxcBlob))
	{
		ASSERT_FMT(this->dxcBlob.Get(), "dxcBlob이 nullptr입니다.");
	}

	inline IDxcBlob& GPUShader::GetDXCBlob() const noexcept
	{
		ASSERT_FMT(dxcBlob.Get(), "dxcBlob이 nullptr입니다.");

		return *dxcBlob.Get();
	}

	inline D3D12_SHADER_BYTECODE GPUShader::GetD3DBytecode() const noexcept
	{
		ASSERT_FMT(dxcBlob.Get(), "dxcBlob이 nullptr입니다.");

		return CD3DX12_SHADER_BYTECODE(dxcBlob->GetBufferPointer(), dxcBlob->GetBufferSize());
	}
}
