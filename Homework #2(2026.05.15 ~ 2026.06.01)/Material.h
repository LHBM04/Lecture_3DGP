#pragma once

#include "Asset.h"
#include "ColorRGBA.h"

class Shader;

class Material final : public Asset
{
public:
	Material() noexcept = default;
	virtual ~Material() noexcept override = default;

	virtual bool Load(const std::filesystem::path& path_) override;
	virtual void Unload() noexcept override;

	[[nodiscard]] const ColorRGBA& GetAlbedoColor() const noexcept;
	void SetAlbedoColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] Shader* GetVertexShader() noexcept;
	[[nodiscard]] const Shader* GetVertexShader() const noexcept;
	void SetVertexShader(Shader* shader_) noexcept;

	[[nodiscard]] Shader* GetPixelShader() noexcept;
	[[nodiscard]] const Shader* GetPixelShader() const noexcept;
	void SetPixelShader(Shader* shader_) noexcept;

	[[nodiscard]] static const Material& GetDefault() noexcept;

private:
	ColorRGBA albedoColor{ ColorRGBA::GetWhite() };
	Shader* vertexShader{ nullptr };
	Shader* pixelShader{ nullptr };
};
