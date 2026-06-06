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
	bool CreatePipelineState(
		ID3D12Device* device_,
		ID3D12RootSignature* rootSignature_,
		DXGI_FORMAT renderTargetFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthStencilFormat_ = DXGI_FORMAT_UNKNOWN);

	[[nodiscard]] ID3DBlob* GetVSBlob() const noexcept;
	[[nodiscard]] ID3DBlob* GetPSBlob() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetPipelineState() const noexcept;
	[[nodiscard]] bool HasPipelineState() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
};
