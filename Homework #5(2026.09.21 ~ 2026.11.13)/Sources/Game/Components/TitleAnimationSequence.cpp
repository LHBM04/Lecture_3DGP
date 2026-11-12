#include "Precompiled.hpp"
#include "TitleAnimationSequence.hpp"

#include "../Core/Time.hpp"

namespace TUK::Game
{
	void TitleAnimationSequence::OnLateUpdate()
	{
		const f32 deltaTime = Time::GetUnscaledDeltaTime();
		constexpr f32 TWO_PI = 2.0f * std::numbers::pi_v<f32>;

		if (nightSkybox && !nightSkybox->IsDestroyed())
		{
			skyboxCycleTime = std::fmod(skyboxCycleTime + deltaTime, SKYBOX_CYCLE_DURATION);
			const f32 phase = TWO_PI * skyboxCycleTime / SKYBOX_CYCLE_DURATION;
			const f32 nightAlpha = 0.5f - 0.5f * std::cos(phase);

			ColorRGBA<f32> nightColor = nightSkybox->GetColor();
			nightColor.SetA(nightAlpha);
			nightSkybox->SetColor(nightColor);
		}

		if (!target || target->IsDestroyed())
		{
			return;
		}

		Transform* cameraTransform = GetOwner()->GetComponent<Transform>();
		const Transform* targetTransform = target->GetComponent<Transform>();
		if (!cameraTransform || !targetTransform)
		{
			return;
		}

		orbitTime = std::fmod(orbitTime + deltaTime, ORBIT_DURATION);
		const f32 angle = TWO_PI * orbitTime / ORBIT_DURATION;
		const Vector3D<f32> offset
		{
			std::sin(angle) * ORBIT_RADIUS,
			0.0f,
			-std::cos(angle) * ORBIT_RADIUS
		};

		cameraTransform->SetWorldPosition(targetTransform->GetWorldPosition() + offset);
		cameraTransform->SetWorldRotation(Quaternion<f32>::LookRotation(-offset, Vector3D<f32>::GetUp()));
	}
}
