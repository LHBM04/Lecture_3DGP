#pragma once

#include "ColorRGBA.h"
#include "UIComponent.h"

class Font;
class Material;

class TextView final : public UIComponent
{
public:
	[[nodiscard]] const std::string& GetText() const noexcept;
	void SetText(std::string text_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] Material* GetMaterial() noexcept;
	[[nodiscard]] const Material* GetMaterial() const noexcept;
	void SetMaterial(Material* material_) noexcept;

	[[nodiscard]] Font* GetFont() noexcept;
	[[nodiscard]] const Font* GetFont() const noexcept;
	void SetFont(Font* font_) noexcept;

	[[nodiscard]] float GetPixelSize() const noexcept;
	void SetPixelSize(float pixelSize_) noexcept;

	[[nodiscard]] float GetCharacterSpacing() const noexcept;
	void SetCharacterSpacing(float characterSpacing_) noexcept;

	[[nodiscard]] float GetLineSpacing() const noexcept;
	void SetLineSpacing(float lineSpacing_) noexcept;

protected:
	virtual void OnRenderUI(Renderer& renderer_) override;

private:
	std::string text{ "Text" };
	Font* font{ nullptr };
	Material* material{ nullptr };
	ColorRGBA color{ ColorRGBA::GetWhite() };
	float pixelSize{ 4.0f };
	float characterSpacing{ 1.0f };
	float lineSpacing{ 2.0f };
};
