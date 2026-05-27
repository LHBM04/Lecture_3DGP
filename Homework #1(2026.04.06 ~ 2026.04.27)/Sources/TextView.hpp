#pragma once

#include "ColorRGBA.hpp"
#include "Component.hpp"

class TextView final : public Component
{
public:
	virtual ~TextView() override = default;

	[[nodiscard]] const std::wstring& GetText() const;
	void SetText(const std::wstring& text_);

	[[nodiscard]] const RECT& GetRect() const;
	void SetRect(const RECT& rect_);

	[[nodiscard]] const ColorRGBA& GetTextColor() const;
	void SetTextColor(const ColorRGBA& color_);

	[[nodiscard]] const ColorRGBA& GetShadowColor() const;
	void SetShadowColor(const ColorRGBA& color_);

	[[nodiscard]] bool IsShadowEnabled() const;
	void SetShadowEnabled(bool enabled_);

	void SetShadowOffset(int x_, int y_);
	void SetFontSize(int size_);
	void SetFontWeight(int weight_);
	void SetFormat(UINT format_);

protected:
	void OnRender() override;
	void OnDetach() override;

private:
	void RecreateFont();

	std::wstring text{ L"" };
	RECT rect{ 0, 0, 0, 0 };

	ColorRGBA textColor{ ColorRGBA::GetWhite() };
	ColorRGBA shadowColor{ ColorRGBA::GetBlack() };
	bool shadowEnabled{ true };
	POINT shadowOffset{ 2, 2 };

	int fontSize{ 32 };
	int fontWeight{ FW_BOLD };
	UINT format{ DT_TOP | DT_LEFT | DT_SINGLELINE };

	HFONT font{ nullptr };
	bool isFontDirty{ true };
};
