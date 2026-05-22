#include "Precompiled.h"
#include "TextView.h"

#include "Font.h"
#include "GameObject.h"
#include "Material.h"
#include "RectTransform.h"
#include "RenderTarget.h"

const std::string& TextView::GetText() const noexcept
{
	return text;
}

void TextView::SetText(std::string text_) noexcept
{
	text = std::move(text_);
}

const ColorRGBA& TextView::GetColor() const noexcept
{
	return color;
}

void TextView::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

Material* TextView::GetMaterial() noexcept
{
	return material;
}

const Material* TextView::GetMaterial() const noexcept
{
	return material;
}

void TextView::SetMaterial(Material* material_) noexcept
{
	material = material_;
}

Font* TextView::GetFont() noexcept
{
	return font;
}

const Font* TextView::GetFont() const noexcept
{
	return font;
}

void TextView::SetFont(Font* font_) noexcept
{
	font = font_;
}

float TextView::GetPixelSize() const noexcept
{
	return pixelSize;
}

void TextView::SetPixelSize(float pixelSize_) noexcept
{
	pixelSize = std::max(1.0f, pixelSize_);
}

float TextView::GetCharacterSpacing() const noexcept
{
	return characterSpacing;
}

void TextView::SetCharacterSpacing(float characterSpacing_) noexcept
{
	characterSpacing = std::max(0.0f, characterSpacing_);
}

float TextView::GetLineSpacing() const noexcept
{
	return lineSpacing;
}

void TextView::SetLineSpacing(float lineSpacing_) noexcept
{
	lineSpacing = std::max(0.0f, lineSpacing_);
}

void TextView::OnRenderUI(RenderTarget& renderTarget_)
{
	const GameObject* const owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	const RectTransform* const rectTransform{ owner->GetComponent<RectTransform>() };
	if (nullptr == rectTransform || nullptr == font)
	{
		return;
	}

	const float left{ rectTransform->GetLeft(static_cast<float>(renderTarget_.GetWidth())) };
	const float top{ rectTransform->GetTop(static_cast<float>(renderTarget_.GetHeight())) };
	const float characterAdvance{ pixelSize * static_cast<float>(font->GetGlyphWidth() + 1) + characterSpacing };
	const float lineAdvance{ pixelSize * static_cast<float>(font->GetGlyphHeight() + 1) + lineSpacing };

	float cursorX{ left };
	float cursorY{ top };
	const ColorRGBA& renderColor{ nullptr != material ? material->GetAlbedoColor() : color };
	for (char character : text)
	{
		if (character == '\n')
		{
			cursorX = left;
			cursorY += lineAdvance;
			continue;
		}

		if (character == ' ')
		{
			cursorX += characterAdvance;
			continue;
		}

		const FontGlyph& glyph{ font->GetGlyph(character) };
		for (int row{ 0 }; row < font->GetGlyphHeight(); ++row)
		{
			for (int column{ 0 }; column < font->GetGlyphWidth(); ++column)
			{
				if ((glyph.rows[static_cast<std::size_t>(row)] & (1 << (font->GetGlyphWidth() - 1 - column))) == 0)
				{
					continue;
				}

				if (nullptr != material)
				{
					renderTarget_.DrawUIRectPixels(
						cursorX + static_cast<float>(column) * pixelSize,
						cursorY + static_cast<float>(row) * pixelSize,
						pixelSize,
						pixelSize,
						renderColor,
						*material);
				}
				else
				{
					renderTarget_.DrawUIRectPixels(
						cursorX + static_cast<float>(column) * pixelSize,
						cursorY + static_cast<float>(row) * pixelSize,
						pixelSize,
						pixelSize,
						renderColor);
				}
			}
		}

		cursorX += characterAdvance;
	}
}
