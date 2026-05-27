#include "Precompiled.hpp"
#include "TextView.hpp"

#include "Renderer.hpp"

const std::wstring& TextView::GetText() const
{
	return text;
}

void TextView::SetText(const std::wstring& text_)
{
	text = text_;
}

const RECT& TextView::GetRect() const
{
	return rect;
}

void TextView::SetRect(const RECT& rect_)
{
	rect = rect_;
}

const ColorRGBA& TextView::GetTextColor() const
{
	return textColor;
}

void TextView::SetTextColor(const ColorRGBA& color_)
{
	textColor = color_;
}

const ColorRGBA& TextView::GetShadowColor() const
{
	return shadowColor;
}

void TextView::SetShadowColor(const ColorRGBA& color_)
{
	shadowColor = color_;
}

bool TextView::IsShadowEnabled() const
{
	return shadowEnabled;
}

void TextView::SetShadowEnabled(bool enabled_)
{
	shadowEnabled = enabled_;
}

void TextView::SetShadowOffset(int x_, int y_)
{
	shadowOffset.x = x_;
	shadowOffset.y = y_;
}

void TextView::SetFontSize(int size_)
{
	fontSize = std::max(size_, 1);
	isFontDirty = true;
}

void TextView::SetFontWeight(int weight_)
{
	fontWeight = weight_;
	isFontDirty = true;
}

void TextView::SetFormat(UINT format_)
{
	format = format_;
}

void TextView::OnRender()
{
	if (text.empty())
	{
		return;
	}

	if (isFontDirty || !font)
	{
		RecreateFont();
	}

	Renderer::TextCommand command{};
	command.text = text;
	command.rect = rect;
	command.textColor = textColor;
	command.shadowColor = shadowColor;
	command.drawShadow = shadowEnabled;
	command.shadowOffset = shadowOffset;
	command.format = format;
	command.font = font;
	Renderer::QueueText(command);
}

void TextView::OnDetach()
{
	if (font)
	{
		DeleteObject(font);
		font = nullptr;
	}
}

void TextView::RecreateFont()
{
	if (font)
	{
		DeleteObject(font);
		font = nullptr;
	}

	font = CreateFontW(
		fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
	isFontDirty = false;
}
