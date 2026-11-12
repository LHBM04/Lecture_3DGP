#include "Precompiled.hpp"
#include "DirectionalLight.hpp"

#include "../Core/Common.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Scene.hpp"
#include "../Objects/Transform.hpp"

#include "../Math/Quaternion.hpp"

namespace TUK::Framework
{
	void DirectionalLight::OnEnable()
	{
		if (GameObject* owner = GetOwner())
		{
			if (Scene* scene = owner->GetScene())
			{
				scene->AddDirectionalLight(this);
			}
		}
	}

	void DirectionalLight::OnDisable()
	{
		if (GameObject* owner = GetOwner())
		{
			if (Scene* scene = owner->GetScene())
			{
				scene->RemoveDirectionalLight(this);
			}
		}
	}

	Vector3D<f32> DirectionalLight::GetDirection() const noexcept
	{
		if (useTransformRotation)
		{
			const GameObject* owner = GetOwner();
			if (owner)
			{
				const Transform* transform = owner->GetComponent<Transform>();
				if (transform)
				{
					const Quaternion<f32> rotation = transform->GetWorldRotation();
					const Vector3D<f32> rotatedDirection = rotation * direction;
					if (!rotatedDirection.IsZero())
					{
						return rotatedDirection.GetNormalized();
					}
				}
			}
		}

		if (!direction.IsZero())
		{
			return direction.GetNormalized();
		}

		return Vector3D<f32>::GetDown();
	}

	DirectionalLightConstants DirectionalLight::GetConstants() const noexcept
	{
		return DirectionalLightConstants
		{
			.direction = GetDirection(),
			.intensity = intensity,
			.color = GetEffectiveColor(),
			.ambientColor = GetEffectiveAmbientColor()
		};
	}
}
