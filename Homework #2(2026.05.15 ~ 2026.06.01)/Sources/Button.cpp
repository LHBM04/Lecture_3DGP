#include "Precompiled.h"
#include "Button.h"

#include "GameObject.h"
#include "InputManager.h"
#include "RectTransform.h"

bool Button::IsInteractable() const noexcept
{
	return interactable;
}

void Button::SetInteractable(bool interactable_) noexcept
{
	interactable = interactable_;
}

bool Button::IsHovered() const noexcept
{
	return hovered;
}

bool Button::IsPressed() const noexcept
{
	return pressed;
}

bool Button::IsSelected() const noexcept
{
	return selected;
}

void Button::SetSelected(bool selected_) noexcept
{
	selected = selected_;
}

void Button::SetOnNormal(Callback callback_)
{
	onNormal = std::move(callback_);
}

void Button::SetOnHighlighted(Callback callback_)
{
	onHighlighted = std::move(callback_);
}

void Button::SetOnPressed(Callback callback_)
{
	onPressed = std::move(callback_);
}

void Button::SetOnSelected(Callback callback_)
{
	onSelected = std::move(callback_);
}

void Button::SetOnDisabled(Callback callback_)
{
	onDisabled = std::move(callback_);
}

void Button::OnAttach()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	if (nullptr == owner->GetComponent<RectTransform>())
	{
		owner->AddComponent<RectTransform>();
	}
}

void Button::OnUpdate()
{
	GameObject* owner{ GetOwner() };
	if (nullptr == owner)
	{
		return;
	}

	RectTransform* rectTransform{ owner->GetComponent<RectTransform>() };
	if (nullptr == rectTransform)
	{
		return;
	}

	if (!interactable)
	{
		hovered = false;
		pressed = false;
		ChangeState(State::Disabled);
		return;
	}

	const auto [mouseX, mouseY]{ InputManager::GetMousePosition() };
	hovered = rectTransform->ContainsScreenPoint(mouseX, mouseY);
	pressed = hovered && InputManager::IsButtonDown(ButtonCode::Left);

	if (pressed)
	{
		ChangeState(State::Pressed);
	}
	else if (hovered)
	{
		ChangeState(State::Highlighted);
	}
	else if (selected)
	{
		ChangeState(State::Selected);
	}
	else
	{
		ChangeState(State::Normal);
	}
}

void Button::ChangeState(State state_)
{
	if (state == state_)
	{
		return;
	}

	state = state_;
	Invoke(state);
}

void Button::Invoke(State state_)
{
	switch (state_)
	{
	case State::Normal:
		if (onNormal)
		{
			onNormal(*this);
		}
		break;

	case State::Highlighted:
		if (onHighlighted)
		{
			onHighlighted(*this);
		}
		break;

	case State::Pressed:
		if (onPressed)
		{
			onPressed(*this);
		}
		break;

	case State::Selected:
		if (onSelected)
		{
			onSelected(*this);
		}
		break;

	case State::Disabled:
		if (onDisabled)
		{
			onDisabled(*this);
		}
		break;

	default:
		break;
	}
}
