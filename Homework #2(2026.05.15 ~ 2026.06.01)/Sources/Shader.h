#pragma once

#include "Resource.h"

#include <d3d12.h>
#include <wrl.h>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

class Shader final : public Resource
{
public:
	Shader() = default;
	virtual ~Shader() = default;

	bool Load(const std::filesystem::path& path_) override;
	void Unload() override;

	[[nodiscard]] uint64_t GetPipelineId() const noexcept;
	void SetPipelineId(uint64_t pipelineId_) noexcept;

	[[nodiscard]] ID3D12PipelineState* GetPipelineState() noexcept;
	[[nodiscard]] const ID3D12PipelineState* GetPipelineState() const noexcept;
	void SetPipelineState(ID3D12PipelineState* pipelineState_) noexcept;

	[[nodiscard]] ID3D12RootSignature* GetGraphicsRootSignature() noexcept;
	[[nodiscard]] const ID3D12RootSignature* GetGraphicsRootSignature() const noexcept;
	void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature_) noexcept;
	[[nodiscard]] bool TryGetRootParameterIndex(const std::string& semanticName_, UINT& outIndex_) const noexcept;

	[[nodiscard]] bool LoadFromFile(ID3D12Device* device_, const std::filesystem::path& path_);

private:
	std::unordered_map<std::string, UINT> rootParameterIndices;
	uint64_t pipelineId{ 0 };
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> graphicsRootSignature;
};
