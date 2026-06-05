#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"
#include "Transform.h"

Light::Type Light::GetType() const noexcept
{
	return type;
}

void Light::SetType(Type type_) noexcept
{
	type = type_;
}

const ColorRGBA& Light::GetColor() const noexcept
{
	return color;
}

void Light::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}

float Light::GetIntensity() const noexcept
{
	return intensity;
}

void Light::SetIntensity(float intensity_) noexcept
{
	intensity = std::max(0.0f, intensity_);
}

float Light::GetRange() const noexcept
{
	return range;
}

void Light::SetRange(float range_) noexcept
{
	range = std::max(0.0f, range_);
}

float Light::GetSpotAngle() const noexcept
{
	return spotAngle;
}

void Light::SetSpotAngle(float spotAngle_) noexcept
{
	spotAngle = std::clamp(spotAngle_, 0.0f, 179.0f);
}

Vector3D Light::GetDirection() const noexcept
{
	return GetOwner().GetTransform().GetWorldMatrix().GetForward();
}

void Light::OnEnable()
{
	GetOwner().GetCurrentScene().AddLight(*this);
}

void Light::OnDisable()
{
	GetOwner().GetCurrentScene().RemoveLight(*this);
}
