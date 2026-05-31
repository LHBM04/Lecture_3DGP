#include "Precompiled.h"

#include "Shader.h"

#include <d3dcompiler.h>

Shader::~Shader()
{
	Unload();
}

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
		path.c_str(), nullptr, nullptr, 
		std::string(vsEntry_.begin(), vsEntry_.end()).c_str(), 
		"vs_5_0", compileFlags, 0, &vsBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
			const std::string errorText(errorChars, errorBlob->GetBufferSize());
			return std::unexpected(std::wstring(errorText.begin(), errorText.end()));
		}
		return std::unexpected(L"Failed to compile Vertex Shader: " + path);
	}

	// Pixel Shader
	hr = D3DCompileFromFile(
		path.c_str(), nullptr, nullptr, 
		std::string(psEntry_.begin(), psEntry_.end()).c_str(), 
		"ps_5_0", compileFlags, 0, &psBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			const char* errorChars{ static_cast<const char*>(errorBlob->GetBufferPointer()) };
			const std::string errorText(errorChars, errorBlob->GetBufferSize());
			return std::unexpected(std::wstring(errorText.begin(), errorText.end()));
		}
		return std::unexpected(L"Failed to compile Pixel Shader: " + path);
	}

	return {};
}

ID3DBlob* Shader::GetVSBlob() const noexcept
{
	return vsBlob.Get();
}

ID3DBlob* Shader::GetPSBlob() const noexcept
{
	return psBlob.Get();
}
