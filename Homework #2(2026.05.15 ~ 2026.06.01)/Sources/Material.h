#pragma once

#include <string_view>

#include "Resource.h"
#include "Vector4D.h"

class Shader;

class Material final : public Resource
{
public:
	Material() = default;
	~Material() override = default;

	bool Load() override;
	void Unload() override;

	[[nodiscard]] Shader* GetShader() const;
	void SetShader(Shader* shader_);

	[[nodiscard]] const Vector4D& GetBaseColor() const;
	void SetBaseColor(const Vector4D& color_);

	[[nodiscard]] float GetRoughness() const;
	void SetRoughness(float roughness_);

	[[nodiscard]] float GetMetallic() const;
	void SetMetallic(float metallic_);

private:
	Shader* shader{ nullptr };
	Vector4D baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
	float roughness{ 0.5f };
	float metallic{ 0.0f };
};

inline Shader* Material::GetShader() const
{
	return shader;
}

inline void Material::SetShader(Shader* shader_)
{
	shader = shader_;
}

inline const Vector4D& Material::GetBaseColor() const
{
	return baseColor;
}

inline void Material::SetBaseColor(const Vector4D& color_)
{
	baseColor = color_;
}

inline float Material::GetRoughness() const
{
	return roughness;
}

inline void Material::SetRoughness(float roughness_)
{
	roughness = roughness_;
}

inline float Material::GetMetallic() const
{
	return metallic;
}

inline void Material::SetMetallic(float metallic_)
{
	metallic = metallic_;
}

