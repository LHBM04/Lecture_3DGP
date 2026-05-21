#pragma once

#include "Asset.h"

class Shader final : public Asset
{
public:
	Shader() noexcept = default;
	virtual ~Shader() noexcept override = default;

	virtual bool Load(const std::filesystem::path& path_) override;
	virtual void Unload() noexcept override;

	[[nodiscard]] ID3DBlob* GetByteCode() const noexcept;
	[[nodiscard]] D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept;
	[[nodiscard]] bool HasByteCode() const noexcept;

	bool Compile(
		const std::filesystem::path& path_,
		const std::string& entryPoint_,
		const std::string& target_);

private:
	Microsoft::WRL::ComPtr<ID3DBlob> byteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> errorMessage;
};
