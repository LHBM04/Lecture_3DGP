#include "Precompiled.h"
#include "Button.h"

#include "GameObject.h"
#include "ImageView.h"
#include "Input.h"
#include "MouseButton.h"
#include "RectTransform.h"

bool Button::IsHovered() const noexcept
{
	return isHovered;
}

bool Button::IsPressed() const noexcept
{
	return isPressed;
}

void Button::SetTargetGraphic(ImageView* targetGraphic_) noexcept
{
	targetGraphic = targetGraphic_;
	UpdateVisual();
}

ImageView* Button::GetTargetGraphic() noexcept
{
	return targetGraphic;
}

const ImageView* Button::GetTargetGraphic() const noexcept
{
	return targetGraphic;
}

void Button::SetNormalColor(const ColorRGBA& color_) noexcept
{
	normalColor = color_;
	UpdateVisual();
}

void Button::SetHoveredColor(const ColorRGBA& color_) noexcept
{
	hoveredColor = color_;
	UpdateVisual();
}

void Button::SetPressedColor(const ColorRGBA& color_) noexcept
{
	pressedColor = color_;
	UpdateVisual();
}

void Button::SetDisabledColor(const ColorRGBA& color_) noexcept
{
	disabledColor = color_;
	UpdateVisual();
}

const ColorRGBA& Button::GetNormalColor() const noexcept
{
	return normalColor;
}

const ColorRGBA& Button::GetHoveredColor() const noexcept
{
	return hoveredColor;
}

const ColorRGBA& Button::GetPressedColor() const noexcept
{
	return pressedColor;
}

const ColorRGBA& Button::GetDisabledColor() const noexcept
{
	return disabledColor;
}

void Button::SetOnClick(std::function<void()> callback_)
{
	onClick = std::move(callback_);
}

void Button::OnAttach()
{
	UIComponent::OnAttach();

	if (nullptr == targetGraphic)
	{
		if (GameObject* const owner{ GetOwner() })
		{
			targetGraphic = owner->GetComponent<ImageView>();
		}
	}

	UpdateVisual();
}

void Button::OnUpdate()
{
	const GameObject* const owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	const RectTransform* const rectTransform{ owner->GetComponent<RectTransform>() };
	if (nullptr == rectTransform)
	{
		return;
	}

	isHovered = ContainsMouse(*rectTransform);

	if (isHovered && Input::IsMouseButtonPressed(ButtonCode::Left))
	{
		isPressed = true;
	}

	if (isPressed && Input::IsMouseButtonReleased(ButtonCode::Left))
	{
		const bool shouldClick{ isHovered };
		isPressed = false;

		if (shouldClick && onClick)
		{
			onClick();
		}
	}

	if (!Input::IsMouseButtonDown(ButtonCode::Left))
	{
		isPressed = false;
	}

	UpdateVisual();
}

bool Button::ContainsMouse(const RectTransform& rectTransform_) const noexcept
{
	const float left{ rectTransform_.GetLeft(static_cast<float>(Input::GetScreenWidth())) };
	const float top{ rectTransform_.GetTop(static_cast<float>(Input::GetScreenHeight())) };
	const float right{ left + rectTransform_.GetWidth() };
	const float bottom{ top + rectTransform_.GetHeight() };
	const float mouseX{ static_cast<float>(Input::GetMouseX()) };
	const float mouseY{ static_cast<float>(Input::GetMouseY()) };

	return left <= mouseX && mouseX <= right && top <= mouseY && mouseY <= bottom;
}

void Button::UpdateVisual() noexcept
{
	if (nullptr == targetGraphic)
	{
		return;
	}

	if (!IsEnabled())
	{
		targetGraphic->SetColor(disabledColor);
		return;
	}

	if (isPressed)
	{
		targetGraphic->SetColor(pressedColor);
		return;
	}

	targetGraphic->SetColor(isHovered ? hoveredColor : normalColor);
}
