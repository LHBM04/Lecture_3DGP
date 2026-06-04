#include "Precompiled.h"
#include "Shader.h"

Shader::Shader(std::wstring_view resourceName_, std::filesystem::path sourcePath_)
	: Resource(resourceName_, std::move(sourcePath_))
{
}

bool Shader::Load()
{
	if (GetPath().empty())
	{
		return false;
	}

	return Compile(GetPath());
}

void Shader::Unload()
{
	vertexShaderBlob.Reset();
	pixelShaderBlob.Reset();
	errorBlob.Reset();
	MarkLoaded(false);
}

bool Shader::Compile(
	std::filesystem::path sourcePath_,
	std::string_view vertexEntryPoint_,
	std::string_view pixelEntryPoint_)
{
	Unload();
	SetSourcePath(std::move(sourcePath_));

	if (GetSourcePath().empty())
	{
		return false;
	}

	UINT compileFlags{ D3DCOMPILE_ENABLE_STRICTNESS };
#if defined(_DEBUG)
	compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	if (FAILED(::D3DCompileFromFile(
		GetSourcePath().c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		vertexEntryPoint_.data(),
		"vs_5_1",
		compileFlags,
		0,
		&vertexShaderBlob,
		&errorBlob)))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderErrors;
	if (FAILED(::D3DCompileFromFile(
		GetSourcePath().c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pixelEntryPoint_.data(),
		"ps_5_1",
		compileFlags,
		0,
		&pixelShaderBlob,
		&pixelShaderErrors)))
	{
		errorBlob = std::move(pixelShaderErrors);
		vertexShaderBlob.Reset();
		return false;
	}

	MarkLoaded(true);
	return true;
}

D3D12_SHADER_BYTECODE Shader::GetVertexShaderBytecode() const noexcept
{
	D3D12_SHADER_BYTECODE bytecode{};
	bytecode.pShaderBytecode = (vertexShaderBlob != nullptr) ? vertexShaderBlob->GetBufferPointer() : nullptr;
	bytecode.BytecodeLength = (vertexShaderBlob != nullptr) ? vertexShaderBlob->GetBufferSize() : 0;
	return bytecode;
}

D3D12_SHADER_BYTECODE Shader::GetPixelShaderBytecode() const noexcept
{
	D3D12_SHADER_BYTECODE bytecode{};
	bytecode.pShaderBytecode = (pixelShaderBlob != nullptr) ? pixelShaderBlob->GetBufferPointer() : nullptr;
	bytecode.BytecodeLength = (pixelShaderBlob != nullptr) ? pixelShaderBlob->GetBufferSize() : 0;
	return bytecode;
}

ID3DBlob* Shader::GetVertexShaderBlob() const noexcept
{
	return vertexShaderBlob.Get();
}

ID3DBlob* Shader::GetPixelShaderBlob() const noexcept
{
	return pixelShaderBlob.Get();
}

ID3DBlob* Shader::GetErrorBlob() const noexcept
{
	return errorBlob.Get();
}
