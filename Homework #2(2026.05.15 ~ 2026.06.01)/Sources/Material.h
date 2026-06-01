#pragma once

#include <string_view>

#include "ColorRGBA.h"
#include "Resource.h"

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

	[[nodiscard]] const ColorRGBA& GetColor() const;
	void SetColor(const ColorRGBA& color_);

private:
	bool ReadTag(std::ifstream& file_, const std::string& expectedTag_);
	std::wstring ReadString(std::ifstream& file_);

	Shader* shader{ nullptr };
	ColorRGBA color{ 1.0f, 1.0f, 1.0f, 1.0f };
};
