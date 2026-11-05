#pragma once

#include <cassert>
#include <expected>
#include <filesystem>
#include <string>
#include <utility>

#include <wrl/client.h>

#include <directx/d3d12.h>
#include <directx/d3dx12.h>
#include <directx-dxc/dxcapi.h>

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	class GPUShader
	{
	public:
		~GPUShader() noexcept = default;

		GPUShader(const GPUShader&) = delete;
		GPUShader& operator=(const GPUShader&) = delete;

		GPUShader(GPUShader&&) noexcept = default;
		GPUShader& operator=(GPUShader&&) noexcept = default;

		[[nodiscard]] IDxcBlob& GetDXCBlob() const noexcept;
		[[nodiscard]] D3D12_SHADER_BYTECODE GetD3DBytecode() const noexcept;

		[[nodiscard]] static std::expected<GPUShader, std::string> Create(const std::filesystem::path& path);

	private:
		explicit GPUShader(ComPtr<IDxcBlob> dxcBlob) noexcept;

		ComPtr<IDxcBlob> dxcBlob;
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

	inline GPUShader::GPUShader(ComPtr<IDxcBlob> dxcBlob) noexcept
		: dxcBlob(std::move(dxcBlob))
	{
	}
}
