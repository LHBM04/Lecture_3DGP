#pragma once

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <d3d12.h>
#include <d3dcompiler.h>

#include <wrl/client.h>

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

		[[nodiscard]] ID3DBlob& GetD3DBlob() const noexcept;
		[[nodiscard]] D3D12_SHADER_BYTECODE GetD3DBytecode() const noexcept;

	private:
		GPUShader(Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob) noexcept;

		Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob;
	};

	inline ID3DBlob& GPUShader::GetD3DBlob() const noexcept
	{
		assert(d3dBlob);
		return *d3dBlob.Get();
	}

	inline D3D12_SHADER_BYTECODE GPUShader::GetD3DBytecode() const noexcept
	{
		assert(d3dBlob);
		return { d3dBlob->GetBufferPointer(), d3dBlob->GetBufferSize() };
	}

	inline GPUShader::GPUShader(Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob) noexcept
		: d3dBlob(std::move(d3dBlob))
	{
	}
}
