#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <expected>
#include <string>
#include <string_view>

#include "Resource.h"

class Shader final : public Resource
{
public:
	Shader() = default;
	~Shader() override = default;

	bool Load() override;
	void Unload() override;

	std::expected<void, std::wstring> Compile(std::wstring_view vsEntry_ = L"VSMain", std::wstring_view psEntry_ = L"PSMain");

	[[nodiscard]] ID3DBlob* GetVSBlob() const;
	[[nodiscard]] ID3DBlob* GetPSBlob() const;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
};

inline ID3DBlob* Shader::GetVSBlob() const
{
	return vsBlob.Get();
}

inline ID3DBlob* Shader::GetPSBlob() const
{
	return psBlob.Get();
}

