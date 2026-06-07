#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"
#include "Vector3D.h"

Vector4D Light::GetLightDirection() const noexcept
{
	const Transform* const transform{ GetOwner() != nullptr ? GetOwner()->GetComponent<Transform>() : nullptr };
	if (transform == nullptr)
	{
		return Vector4D(0.0f, -1.0f, 0.0f, 0.0f);
	}

	const Vector3D forward{ transform->GetWorldMatrix().GetForward().GetNormalized() };
	return Vector4D(-forward.x, -forward.y, -forward.z, 0.0f);
}

const ColorRGBA& Light::GetColor() const noexcept
{
	return color;
}

void Light::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

void Light::OnEnable()
{
	if (GetOwner() != nullptr && GetOwner()->GetScene() != nullptr)
	{
		GetOwner()->GetScene()->AddLight(this);
	}
}

void Light::OnDisable()
{
	if (GetOwner() != nullptr && GetOwner()->GetScene() != nullptr)
	{
		GetOwner()->GetScene()->RemoveLight(this);
	}
}
