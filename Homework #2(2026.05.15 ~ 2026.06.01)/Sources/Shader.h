#pragma once

#include <d3d12.h>
#include <expected>
#include <string>
#include <string_view>
#include <wrl.h>

#include "Resource.h"

class Shader final : public Resource
{
public:
	Shader() = default;
	~Shader() override;

	bool Load() override;
	void Unload() override;

	std::expected<void, std::wstring> Compile(std::wstring_view vsEntry_ = L"VSMain", std::wstring_view psEntry_ = L"PSMain");

	[[nodiscard]] ID3DBlob* GetVSBlob() const noexcept;
	[[nodiscard]] ID3DBlob* GetPSBlob() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
};
