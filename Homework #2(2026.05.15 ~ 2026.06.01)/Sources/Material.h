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

private:
	bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	std::wstring ReadString(std::ifstream& file_);

	Shader* shader{ nullptr };
	Vector4D baseColor{ 1.0f, 1.0f, 1.0f, 1.0f };
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

