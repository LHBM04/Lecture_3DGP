#pragma once

#include "ColorRGBA.h"
#include "UIComponent.h"

class UIRectRenderer final : public UIComponent
{
public:
	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

protected:
	virtual void OnRenderUI(Renderer& renderer_) override;

private:
	ColorRGBA color{ ColorRGBA::GetWhite() };
};
