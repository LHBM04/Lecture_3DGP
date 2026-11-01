#pragma once

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <directx/d3d12.h>
#include <directx/d3dx12.h>

#include <wrl/client.h>

#include <dxcapi.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUShader
	{
		friend class GPUDevice;
		
	public:
		~GPUShader() noexcept = default;

		GPUShader(const GPUShader&) = delete;
		GPUShader& operator=(const GPUShader&) = delete;

		GPUShader(GPUShader&&) noexcept = default;
		GPUShader& operator=(GPUShader&&) noexcept = default;

		[[nodiscard]] IDxcBlob& GetDXCBlob() const noexcept;
		[[nodiscard]] D3D12_SHADER_BYTECODE GetD3DBytecode() const noexcept;

	private:
		GPUShader(Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob) noexcept;

		Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob;
	};

	inline IDxcBlob& GPUShader::GetDXCBlob() const noexcept
	{
		assert(dxcBlob);
		return *dxcBlob.Get();
	}

	inline D3D12_SHADER_BYTECODE GPUShader::GetD3DBytecode() const noexcept
	{
		assert(dxcBlob);
		return CD3DX12_SHADER_BYTECODE(dxcBlob->GetBufferPointer(), dxcBlob->GetBufferSize());
	}

	inline GPUShader::GPUShader(Microsoft::WRL::ComPtr<IDxcBlob> dxcBlob) noexcept
		: dxcBlob(std::move(dxcBlob))
	{
	}
}
