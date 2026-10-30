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
	class D3DShader
	{
		friend class D3DDevice;
		
	public:
		~D3DShader() noexcept = default;

		D3DShader(const D3DShader&) = delete;
		D3DShader& operator=(const D3DShader&) = delete;

		D3DShader(D3DShader&&) noexcept = default;
		D3DShader& operator=(D3DShader&&) noexcept = default;

		[[nodiscard]] ID3DBlob& GetD3DBlob() const noexcept;
		[[nodiscard]] D3D12_SHADER_BYTECODE GetD3DBytecode() const noexcept;

	private:
		D3DShader(Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob) noexcept;

		Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob;
	};

	inline ID3DBlob& D3DShader::GetD3DBlob() const noexcept
	{
		assert(d3dBlob);
		return *d3dBlob.Get();
	}

	inline D3D12_SHADER_BYTECODE D3DShader::GetD3DBytecode() const noexcept
	{
		assert(d3dBlob);
		return { d3dBlob->GetBufferPointer(), d3dBlob->GetBufferSize() };
	}

	inline D3DShader::D3DShader(Microsoft::WRL::ComPtr<ID3DBlob> d3dBlob) noexcept
		: d3dBlob(std::move(d3dBlob))
	{
	}
}
