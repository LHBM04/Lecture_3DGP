#pragma once

#include <filesystem>
#include <string_view>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "Resource.h"

class Shader final : public Resource
{
public:
	Shader(std::wstring_view resourceName_ = {}, std::filesystem::path sourcePath_ = {});
	~Shader() override = default;

	bool Load() override;
	void Unload() override;

	bool Compile(
		std::filesystem::path sourcePath_,
		std::string_view vertexEntryPoint_ = "VSMain",
		std::string_view pixelEntryPoint_ = "PSMain");

	[[nodiscard]] D3D12_SHADER_BYTECODE GetVertexShaderBytecode() const noexcept;
	[[nodiscard]] D3D12_SHADER_BYTECODE GetPixelShaderBytecode() const noexcept;

	[[nodiscard]] ID3DBlob* GetVertexShaderBlob() const noexcept;
	[[nodiscard]] ID3DBlob* GetPixelShaderBlob() const noexcept;
	[[nodiscard]] ID3DBlob* GetErrorBlob() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
};
