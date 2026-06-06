#pragma once

#include <string_view>

#include "ColorRGBA.h"
#include "Resource.h"

class Shader;

struct alignas(256) MaterialConstants final
{
	ColorRGBA baseColor;
	ColorRGBA emissiveColor;
	float metallic{ 0.0f };
	float roughness{ 0.5f };
	float padding[2]{};
};

class Material final : public Resource
{
public:
	Material() = default;
	~Material() override = default;

	bool Load() override;
	void Unload() override;

	[[nodiscard]] Shader* GetShader() const;
	void SetShader(Shader* shader_);

	[[nodiscard]] const ColorRGBA& GetAlbedoColor() const;
	void SetAlbedoColor(const ColorRGBA& color_);

	[[nodiscard]] const ColorRGBA& GetBaseColor() const;
	void SetBaseColor(const ColorRGBA& color_);

	[[nodiscard]] const ColorRGBA& GetEmissiveColor() const;
	void SetEmissiveColor(const ColorRGBA& color_);

	[[nodiscard]] const ColorRGBA& GetSpecularColor() const;
	void SetSpecularColor(const ColorRGBA& color_);

	[[nodiscard]] float GetGlossiness() const noexcept;
	void SetGlossiness(float glossiness_) noexcept;

	[[nodiscard]] float GetSmoothness() const noexcept;
	void SetSmoothness(float smoothness_) noexcept;

	[[nodiscard]] float GetMetallic() const noexcept;
	void SetMetallic(float metallic_) noexcept;

	[[nodiscard]] float GetSpecularHighlight() const noexcept;
	void SetSpecularHighlight(float specularHighlight_) noexcept;

	[[nodiscard]] float GetGlossyReflection() const noexcept;
	void SetGlossyReflection(float glossyReflection_) noexcept;

	[[nodiscard]] float GetRoughness() const noexcept;
	void SetRoughness(float roughness_) noexcept;

private:
	Shader* shader{ nullptr };
	ColorRGBA albedoColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	ColorRGBA emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	ColorRGBA specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	float glossiness{ 0.5f };
	float smoothness{ 0.5f };
	float metallic{ 0.0f };
	float specularHighlight{ 0.0f };
	float glossyReflection{ 0.0f };
	float roughness{ 0.5f };
};
