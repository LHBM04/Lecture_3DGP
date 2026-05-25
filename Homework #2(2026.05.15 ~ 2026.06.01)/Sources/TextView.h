#pragma once

#include "ColorRGBA.h"
#include "Component.h"

#include <string>

class RectTransform;
class Font;
class Material;
class Mesh;

class TextView final : public Component
{
public:
	~TextView() override = default;

	[[nodiscard]] const std::wstring& GetText() const noexcept;
	void SetText(const std::wstring& text_) noexcept;

	[[nodiscard]] Font* GetFont() const noexcept;
	void SetFont(Font* font_) noexcept;

	[[nodiscard]] Mesh* GetMesh() const noexcept;
	void SetMesh(Mesh* mesh_) noexcept;

	[[nodiscard]] Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

protected:
	void OnAttach() override;
	void OnRender() override;

private:
	std::wstring text{ L"TextView" };
	Font* font{ nullptr };
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };
	bool hasLoggedMissingFont{ false };
};
