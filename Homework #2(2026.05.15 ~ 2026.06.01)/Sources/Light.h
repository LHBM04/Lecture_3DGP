#pragma once

#include "Component.h"
#include "ColorRGBA.h"

class Light : public Component
{
public:
	Light() = default;
	~Light() override = default;

	[[nodiscard]] float GetIntensity() const noexcept;
	void SetIntensity(float intensity_) noexcept;

	[[nodiscard]] ColorRGBA GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

protected:
	void OnPreRender() override;
	void OnPostRender() override;

private:
	float intensity{ 1.0f };
	ColorRGBA color{ ColorRGBA::GetWhite()};
};

inline float Light::GetIntensity() const noexcept
{
	return intensity;
}

inline void Light::SetIntensity(float intensity_) noexcept
{
	intensity = intensity_;
}

inline ColorRGBA Light::GetColor() const noexcept
{
	return color;
}

inline void Light::SetColor(const ColorRGBA& color_) noexcept
{
	color = color_;
}
