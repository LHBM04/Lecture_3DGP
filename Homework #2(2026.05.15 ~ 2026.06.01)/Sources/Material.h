#pragma once

#include "Resource.h"
#include "ColorRGBA.h"

#include <d3d12.h>
#include <cstdint>

class Shader;

class Material final : public Resource
{
public:
	Material() = default;
	virtual ~Material() = default;

	bool Load(const std::filesystem::path& path_) override;
	void Unload() override;

	[[nodiscard]] uint64_t GetId() const noexcept;
	void SetId(uint64_t id_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] Shader* GetShader() noexcept;
	[[nodiscard]] const Shader* GetShader() const noexcept;
	void SetShader(Shader* shader_) noexcept;

	[[nodiscard]] D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorTable() const noexcept;
	void SetDescriptorTable(D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable_) noexcept;

private:
	uint64_t id{ 0 };
	ColorRGBA color{ ColorRGBA::GetWhite() };
	Shader* shader{ nullptr };
	D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable{};
};
