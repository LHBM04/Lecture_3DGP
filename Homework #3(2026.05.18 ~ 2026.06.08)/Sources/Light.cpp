#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

float Light::GetIntensity() const noexcept
{
	return intensity;
}

void Light::SetIntensity(float intensity_) noexcept
{
	intensity = std::max(0.0f, intensity_);
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
	if (GameObject* const owner{ GetOwner() }; owner != nullptr && owner->GetScene() != nullptr)
	{
		owner->GetScene()->AddLight(this);
	}
}

void Light::OnDisable()
{
	if (GameObject* const owner{ GetOwner() }; owner != nullptr && owner->GetScene() != nullptr)
	{
		owner->GetScene()->RemoveLight(this);
	}
}
