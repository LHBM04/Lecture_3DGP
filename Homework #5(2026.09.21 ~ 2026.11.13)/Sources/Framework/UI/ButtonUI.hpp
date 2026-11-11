#pragma once

#include "../Core/Common.hpp"

#include "../Objects/Component.hpp"

namespace TUK::Framework
{
	class ButtonUI final : public Component
	{
	public:
		using Component::Component;

		void OnUpdate() override;
		void OnDisable() override;

		[[nodiscard]] bool IsHovered() const noexcept;
		[[nodiscard]] bool IsPressed() const noexcept;

		[[nodiscard]] Delegate<>& OnEnter() noexcept;
		[[nodiscard]] Delegate<>& OnExit() noexcept;
		[[nodiscard]] Delegate<>& OnDown() noexcept;
		[[nodiscard]] Delegate<>& OnUp() noexcept;
		[[nodiscard]] Delegate<>& OnClick() noexcept;

	private:
		Delegate<> onEnter{};
		Delegate<> onExit{};
		Delegate<> onDown{};
		Delegate<> onUp{};
		Delegate<> onClick{};

		bool isHovered = false;
		bool isPressed = false;
	};

	inline Delegate<>& ButtonUI::OnEnter() noexcept
	{
		return onEnter;
	}

	inline Delegate<>& ButtonUI::OnExit() noexcept
	{
		return onExit;
	}

	inline Delegate<>& ButtonUI::OnDown() noexcept
	{
		return onDown;
	}

	inline Delegate<>& ButtonUI::OnUp() noexcept
	{
		return onUp;
	}

	inline Delegate<>& ButtonUI::OnClick() noexcept
	{
		return onClick;
	}

	inline bool ButtonUI::IsHovered() const noexcept
	{
		return isHovered;
	}

	inline bool ButtonUI::IsPressed() const noexcept
	{
		return isPressed;
	}
}
