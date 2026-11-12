#pragma once

#include "Framework/GameObjects.hpp"
#include "Framework/Graphics/SkyboxRenderer.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class TitleAnimationSequence final : public Component
	{
	public:
		using Component::Component;

		void OnLateUpdate() override;

		[[nodiscard]] const GameObject* GetTarget() const noexcept;
		void SetTarget(const GameObject* value) noexcept;

		[[nodiscard]] const SkyboxRenderer* GetNightSkybox() const noexcept;
		void SetNightSkybox(SkyboxRenderer* value) noexcept;

	private:
		static constexpr f32 ORBIT_RADIUS = 10.0f;
		static constexpr f32 ORBIT_DURATION = 90.0f;
		static constexpr f32 SKYBOX_CYCLE_DURATION = 30.0f;

		const GameObject* target = nullptr;
		SkyboxRenderer* nightSkybox = nullptr;
		f32 orbitTime = 0.0f;
		f32 skyboxCycleTime = 0.0f;
	};

	inline const GameObject* TitleAnimationSequence::GetTarget() const noexcept
	{
		return target;
	}

	inline void TitleAnimationSequence::SetTarget(const GameObject* value) noexcept
	{
		target = value;
		orbitTime = 0.0f;
	}

	inline const SkyboxRenderer* TitleAnimationSequence::GetNightSkybox() const noexcept
	{
		return nightSkybox;
	}

	inline void TitleAnimationSequence::SetNightSkybox(SkyboxRenderer* value) noexcept
	{
		nightSkybox = value;
		skyboxCycleTime = 0.0f;
		if (nightSkybox)
		{
			ColorRGBA<f32> nightColor = nightSkybox->GetColor();
			nightColor.SetA(0.0f);
			nightSkybox->SetColor(nightColor);
		}
	}
}
