#include "Precompiled.h"
#include "Shader.h"

#include <d3dcompiler.h>

bool Shader::Load()
{
	return Compile().has_value();
}

void Shader::Unload()
{
	vsBlob.Reset();
	psBlob.Reset();
}

std::expected<void, std::wstring> Shader::Compile(std::wstring_view vsEntry_, std::wstring_view psEntry_)
{
	UINT compileFlags{ 0 };
#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	// Vertex Shader
	HRESULT hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		std::string(vsEntry_.begin(), vsEntry_.end()).c_str(),
		"vs_5_0",
		compileFlags,
		0,
		&vsBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			return std::unexpected(std::wstring(static_cast<const wchar_t*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()));
		}
		return std::unexpected(L"Failed to compile Vertex Shader: " + path.wstring());
	}

	// Pixel Shader
	hr = D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		std::string(psEntry_.begin(), psEntry_.end()).c_str(),
		"ps_5_0",
		compileFlags,
		0,
		&psBlob,
		&errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			return std::unexpected(std::wstring(static_cast<const wchar_t*>(errorBlob->GetBufferPointer()), errorBlob->GetBufferSize()));
		}
		return std::unexpected(L"Failed to compile Pixel Shader: " + path.wstring());
	}

	return {};
}

