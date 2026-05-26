#pragma once

#include "Component.h"
#include "Updatable.h"

#include <functional>

class RectTransform;
struct TimeContext;

class Button final : public Component, public Updatable
{
public:
	using Callback = std::function<void(Button&)>;

	Button() = default;
	~Button() override = default;

	[[nodiscard]] bool IsInteractable() const noexcept;
	void SetInteractable(bool interactable_) noexcept;

	[[nodiscard]] bool IsHovered() const noexcept;
	[[nodiscard]] bool IsPressed() const noexcept;
	[[nodiscard]] bool IsSelected() const noexcept;
	void SetSelected(bool selected_) noexcept;

	void SetOnNormal(Callback callback_);
	void SetOnHighlighted(Callback callback_);
	void SetOnPressed(Callback callback_);
	void SetOnSelected(Callback callback_);
	void SetOnDisabled(Callback callback_);

protected:
	void OnAttach() override;
	void OnUpdate(const TimeContext& context_) override;

private:
	enum class State : unsigned char
	{
		None,
		Normal,
		Highlighted,
		Pressed,
		Selected,
		Disabled,
	};

	void ChangeState(State state_);
	void Invoke(State state_);

private:
	bool interactable{ true };
	bool hovered{ false };
	bool pressed{ false };
	bool selected{ false };

	State state{ State::None };

	Callback onNormal;
	Callback onHighlighted;
	Callback onPressed;
	Callback onSelected;
	Callback onDisabled;
};
