#pragma once

#include "../Core/Common.hpp"

#include "../Math/Vector2D.hpp"

#include "InputProvider.hpp"
#include "InputStates.hpp"

namespace TUK::Framework
{
	enum class InputPhase : u8
	{
		STARTED,
		PERFORMED,
		CANCELED
	};

	struct InputActionContext final
	{
		InputPhase phase = InputPhase::STARTED;
		Vector2D<f32> value{};
	};

	class InputAction final
	{
	public:
		using ActionCallback = Delegate<const InputActionContext&>;

		explicit InputAction(std::string_view name) noexcept;
		~InputAction() noexcept = default;

		InputAction(const InputAction&) = delete;
		InputAction& operator=(const InputAction&) = delete;

		InputAction(InputAction&&) noexcept = default;
		InputAction& operator=(InputAction&&) noexcept = default;

		void AddButtonBinding(KeyCode key);
		void AddMouseButtonBinding(ButtonCode button);
		void AddAxis1DBinding(KeyCode negative, KeyCode positive);
		void AddVector2DBinding(KeyCode up, KeyCode down, KeyCode left, KeyCode right);
		void ClearBindings() noexcept;

		void Evaluate(const InputProvider& provider);
		void Reset() noexcept;

		[[nodiscard]] const std::string& GetName() const noexcept;
		[[nodiscard]] bool IsActive() const noexcept;
		[[nodiscard]] const Vector2D<f32>& GetValue() const noexcept;

		[[nodiscard]] ActionCallback& OnStarted() noexcept;
		[[nodiscard]] ActionCallback& OnPerformed() noexcept;
		[[nodiscard]] ActionCallback& OnCanceled() noexcept;

	private:
		enum class BindingKind : u8
		{
			KEY_BUTTON,
			MOUSE_BUTTON,
			AXIS_1D,
			VECTOR_2D
		};

		struct Binding final
		{
			BindingKind kind = BindingKind::KEY_BUTTON;
			KeyCode primaryKey = KeyCode::NONE;
			KeyCode secondaryKey = KeyCode::NONE;
			KeyCode leftKey = KeyCode::NONE;
			KeyCode rightKey = KeyCode::NONE;
			ButtonCode mouseButton = ButtonCode::NONE;
		};

		std::string name;
		std::vector<Binding> bindings;
		Vector2D<f32> currentValue{};
		bool isActive = false;

		ActionCallback onStarted;
		ActionCallback onPerformed;
		ActionCallback onCanceled;
	};

	inline InputAction::InputAction(std::string_view name) noexcept
		: name(name)
	{
	}

	inline void InputAction::AddButtonBinding(KeyCode key)
	{
		bindings.push_back(Binding
		{
			.kind = BindingKind::KEY_BUTTON,
			.primaryKey = key
		});
	}

	inline void InputAction::AddMouseButtonBinding(ButtonCode button)
	{
		bindings.push_back(Binding
		{
			.kind = BindingKind::MOUSE_BUTTON,
			.mouseButton = button
		});
	}

	inline void InputAction::AddAxis1DBinding(KeyCode negative, KeyCode positive)
	{
		bindings.push_back(Binding
		{
			.kind = BindingKind::AXIS_1D,
			.primaryKey = negative,
			.secondaryKey = positive
		});
	}

	inline void InputAction::AddVector2DBinding(KeyCode up, KeyCode down, KeyCode left, KeyCode right)
	{
		bindings.push_back(Binding
		{
			.kind = BindingKind::VECTOR_2D,
			.primaryKey = up,
			.secondaryKey = down,
			.leftKey = left,
			.rightKey = right
		});
	}

	inline void InputAction::ClearBindings() noexcept
	{
		bindings.clear();
	}

	inline void InputAction::Evaluate(const InputProvider& provider)
	{
		f32 x = 0.0f;
		f32 y = 0.0f;

		for (const Binding& binding : bindings)
		{
			switch (binding.kind)
			{
				case BindingKind::KEY_BUTTON:
				{
					if (provider.IsKeyHeld(binding.primaryKey))
					{
						x = 1.0f;
					}
					break;
				}
				case BindingKind::MOUSE_BUTTON:
				{
					if (provider.HasMouseButtonState() && provider.IsMouseButtonHeld(binding.mouseButton))
					{
						x = 1.0f;
					}
					break;
				}
				case BindingKind::AXIS_1D:
				{
					const f32 axis =
						static_cast<f32>(provider.IsKeyHeld(binding.secondaryKey)) -
						static_cast<f32>(provider.IsKeyHeld(binding.primaryKey));
					x += axis;
					break;
				}
				case BindingKind::VECTOR_2D:
				{
					const f32 horizontal =
						static_cast<f32>(provider.IsKeyHeld(binding.rightKey)) -
						static_cast<f32>(provider.IsKeyHeld(binding.leftKey));
					const f32 vertical =
						static_cast<f32>(provider.IsKeyHeld(binding.primaryKey)) -
						static_cast<f32>(provider.IsKeyHeld(binding.secondaryKey));
					x += horizontal;
					y += vertical;
					break;
				}
			}
		}

		x = std::clamp(x, -1.0f, 1.0f);
		y = std::clamp(y, -1.0f, 1.0f);

		constexpr f32 EPSILON = 1.0e-5f;
		const bool activeNow = (std::abs(x) > EPSILON) || (std::abs(y) > EPSILON);
		const bool wasActive = isActive;

		currentValue = activeNow ? Vector2D<f32>(x, y) : Vector2D<f32>{};
		isActive = activeNow;

		if (!wasActive && activeNow)
		{
			onStarted.Broadcast(InputActionContext{ InputPhase::STARTED, currentValue });
			onPerformed.Broadcast(InputActionContext{ InputPhase::PERFORMED, currentValue });
		}
		else if (wasActive && activeNow)
		{
			onPerformed.Broadcast(InputActionContext{ InputPhase::PERFORMED, currentValue });
		}
		else if (wasActive && !activeNow)
		{
			onCanceled.Broadcast(InputActionContext{ InputPhase::CANCELED, Vector2D<f32>{} });
		}
	}

	inline void InputAction::Reset() noexcept
	{
		if (isActive)
		{
			isActive = false;
			currentValue = {};
			onCanceled.Broadcast(InputActionContext{ InputPhase::CANCELED, Vector2D<f32>{} });
		}
	}

	inline const std::string& InputAction::GetName() const noexcept
	{
		return name;
	}

	inline bool InputAction::IsActive() const noexcept
	{
		return isActive;
	}

	inline const Vector2D<f32>& InputAction::GetValue() const noexcept
	{
		return currentValue;
	}

	inline InputAction::ActionCallback& InputAction::OnStarted() noexcept
	{
		return onStarted;
	}

	inline InputAction::ActionCallback& InputAction::OnPerformed() noexcept
	{
		return onPerformed;
	}

	inline InputAction::ActionCallback& InputAction::OnCanceled() noexcept
	{
		return onCanceled;
	}
}
