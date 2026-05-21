#include "Precompiled.h"
#include "Shader.h"

bool Shader::Load(const std::filesystem::path& path_)
{
	Unload();

	if (FAILED(D3DReadFileToBlob(path_.c_str(), &byteCode)))
	{
		return false;
	}

	SetPath(path_);
	SetLoaded(true);
	return true;
}

void Shader::Unload() noexcept
{
	ClearPath();
	SetLoaded(false);
	byteCode.Reset();
	errorMessage.Reset();
}

ID3DBlob* Shader::GetByteCode() const noexcept
{
	return byteCode.Get();
}

D3D12_SHADER_BYTECODE Shader::GetShaderByteCode() const noexcept
{
	D3D12_SHADER_BYTECODE shaderByteCode{};
	shaderByteCode.pShaderBytecode = nullptr != byteCode ? byteCode->GetBufferPointer() : nullptr;
	shaderByteCode.BytecodeLength = nullptr != byteCode ? byteCode->GetBufferSize() : 0;
	return shaderByteCode;
}

bool Shader::HasByteCode() const noexcept
{
	return nullptr != byteCode && byteCode->GetBufferPointer() != nullptr && byteCode->GetBufferSize() > 0;
}

bool Shader::Compile(
	const std::filesystem::path& path_,
	const std::string& entryPoint_,
	const std::string& target_)
{
	Unload();

	UINT flags{ D3DCOMPILE_ENABLE_STRICTNESS };
#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	const HRESULT result{ D3DCompileFromFile(
		path_.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint_.c_str(),
		target_.c_str(),
		flags,
		0,
		&byteCode,
		&errorMessage) };

	if (FAILED(result))
	{
		byteCode.Reset();
		return false;
	}

	SetPath(path_);
	SetLoaded(true);
	return true;
}
