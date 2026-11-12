#pragma once

#include "../Core/Common.hpp"

#include "../Objects/Component.hpp"

namespace TUK::Framework
{
#ifdef _DEBUG
	class MaterialAsset;
#endif

	class ButtonUI final : public Component
	{
		using Component::Component;

	public:
		void OnUpdate() override;
		void OnDisable() override;
#ifdef _DEBUG
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const MaterialAsset* GetDebugMaterial() const noexcept;
		void SetDebugMaterial(const MaterialAsset* value) noexcept;
#endif

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
#ifdef _DEBUG
		const MaterialAsset* debugMaterial = nullptr;
#endif
	};

#ifdef _DEBUG
	inline const MaterialAsset* ButtonUI::GetDebugMaterial() const noexcept
	{
		return debugMaterial;
	}

	inline void ButtonUI::SetDebugMaterial(const MaterialAsset* value) noexcept
	{
		debugMaterial = value;
	}
#endif

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
