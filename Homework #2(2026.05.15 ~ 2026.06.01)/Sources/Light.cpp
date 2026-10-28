#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"

float Light::GetIntensity() const
{
	return intensity;
}

void Light::SetIntensity(float intensity_)
{
	intensity = intensity_;
}

ColorRGBA Light::GetColor() const
{
	return color;
}

void Light::SetColor(const ColorRGBA& color_)
{
	color = color_;
}

void Light::OnEnable()
{
	GetOwner()->GetScene()->AddLight(this);
}

void Light::OnDisable()
{
	GetOwner()->GetScene()->RemoveLight(this);
}

