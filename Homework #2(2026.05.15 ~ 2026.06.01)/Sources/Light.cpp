#include "Precompiled.h"
#include "Light.h"

#include <algorithm>

Light::Type Light::GetType() const noexcept
{
	return type;
}

void Light::SetType(Type type_) noexcept
{
	type = type_;
}

const ColorRGB& Light::GetColor() const noexcept
{
	return color;
}

void Light::SetColor(const ColorRGB& color_) noexcept
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

const Vector3D& Light::GetDirection() const noexcept
{
	return direction;
}

void Light::SetDirection(const Vector3D& direction_) noexcept
{
	if (!direction_.IsZero())
	{
		direction = direction_.GetNormalized();
	}
}
