#pragma once

#include <span>
#include <string>
#include <vector>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include "ColorRGBA.h"
#include "Resource.h"

class Shader;

struct alignas(256) MaterialConstants final
{
	ColorRGBA albedoColor{ ColorRGBA::GetWhite() };
};

class Material final : public Resource
{
public:
	struct InputElement final
	{
		std::string semanticName;
		UINT semanticIndex{ 0 };
		DXGI_FORMAT format{ DXGI_FORMAT_R32G32B32_FLOAT };
		UINT inputSlot{ 0 };
		UINT alignedByteOffset{ D3D12_APPEND_ALIGNED_ELEMENT };
		D3D12_INPUT_CLASSIFICATION inputSlotClass{ D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA };
		UINT instanceDataStepRate{ 0 };
	};

	Material(std::wstring_view resourceName_ = {}, std::filesystem::path sourcePath_ = {});
	~Material() override = default;

	bool Load() override;
	void Unload() override;

	void SetShader(Shader* shader_) noexcept;
	[[nodiscard]] Shader* GetShader() noexcept;
	[[nodiscard]] const Shader* GetShader() const noexcept;

	void SetColor(const ColorRGBA& color_) noexcept;
	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;

	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_) noexcept;
	[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const noexcept;

	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType_) noexcept;
	[[nodiscard]] D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType() const noexcept;

	void SetFillMode(D3D12_FILL_MODE fillMode_) noexcept;
	void SetCullMode(D3D12_CULL_MODE cullMode_) noexcept;
	void SetDepthEnabled(bool isEnabled_) noexcept;
	void SetBlendEnabled(bool isEnabled_) noexcept;

	void SetInputLayout(std::span<const InputElement> inputElements_);
	[[nodiscard]] const std::vector<InputElement>& GetInputLayout() const noexcept;

	bool BuildPipelineState(
		DXGI_FORMAT renderTargetFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthStencilFormat_ = DXGI_FORMAT_D24_UNORM_S8_UINT);

	void Bind(ID3D12GraphicsCommandList* commandList_) const;
	[[nodiscard]] MaterialConstants GetMaterialConstants() const noexcept;

	[[nodiscard]] ID3D12RootSignature* GetRootSignature() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetPipelineState() const noexcept;

private:
	static bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	static std::wstring ReadString(std::ifstream& file_);

	Shader* shader{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
	D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType{ D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE };
	D3D12_FILL_MODE fillMode{ D3D12_FILL_MODE_SOLID };
	D3D12_CULL_MODE cullMode{ D3D12_CULL_MODE_BACK };
	bool isDepthEnabled{ true };
	bool isBlendEnabled{ false };

	std::vector<InputElement> inputElements;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature;
	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrors;
};
