#pragma once

#include "ColorRGBA.h"
#include "UIComponent.h"

class ImageView;
class RectTransform;

class Button final : public UIComponent
{
public:
	[[nodiscard]] bool IsHovered() const noexcept;
	[[nodiscard]] bool IsPressed() const noexcept;

	void SetTargetGraphic(ImageView* targetGraphic_) noexcept;
	[[nodiscard]] ImageView* GetTargetGraphic() noexcept;
	[[nodiscard]] const ImageView* GetTargetGraphic() const noexcept;

	void SetNormalColor(const ColorRGBA& color_) noexcept;
	void SetHoveredColor(const ColorRGBA& color_) noexcept;
	void SetPressedColor(const ColorRGBA& color_) noexcept;
	void SetDisabledColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] const ColorRGBA& GetNormalColor() const noexcept;
	[[nodiscard]] const ColorRGBA& GetHoveredColor() const noexcept;
	[[nodiscard]] const ColorRGBA& GetPressedColor() const noexcept;
	[[nodiscard]] const ColorRGBA& GetDisabledColor() const noexcept;

	void SetOnClick(std::function<void()> callback_);

protected:
	virtual void OnAttach() override;
	virtual void OnUpdate() override;

private:
	[[nodiscard]] bool ContainsMouse(const RectTransform& rectTransform_) const noexcept;
	void UpdateVisual() noexcept;

	ImageView* targetGraphic{ nullptr };
	std::function<void()> onClick;

	ColorRGBA normalColor{ ColorRGBA(0.20f, 0.45f, 0.90f, 0.90f) };
	ColorRGBA hoveredColor{ ColorRGBA(0.30f, 0.58f, 1.00f, 0.95f) };
	ColorRGBA pressedColor{ ColorRGBA(0.12f, 0.30f, 0.72f, 1.00f) };
	ColorRGBA disabledColor{ ColorRGBA(0.35f, 0.35f, 0.35f, 0.65f) };

	bool isHovered{ false };
	bool isPressed{ false };
};
