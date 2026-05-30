#pragma once
#include "Component.h"
#include "ColorRGBA.h"

class Light final : public Component
{
public:
	Light() = default;
	~Light() override = default;

	[[nodiscard]] float GetIntensity() const;
	void SetIntensity(float intensity_);

	[[nodiscard]] ColorRGBA GetColor() const;
	void SetColor(const ColorRGBA& color_);

	void OnEnable() override;
	void OnDisable() override;

private:
	float intensity{ 1.0f };
	ColorRGBA color{ ColorRGBA::GetWhite() };
};

inline float Light::GetIntensity() const
{
	return intensity;
}

inline void Light::SetIntensity(float intensity_)
{
	intensity = intensity_;
}

inline ColorRGBA Light::GetColor() const
{
	return color;
}

inline void Light::SetColor(const ColorRGBA& color_)
{
	color = color_;
}

