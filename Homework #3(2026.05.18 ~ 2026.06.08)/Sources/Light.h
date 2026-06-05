#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "Vector4D.h"

struct alignas(256) LightConstants final
{
	ColorRGBA ambientColor;
	ColorRGBA lightColor;
	Vector4D lightDirection;
};

class Light final : public Component
{
public:
	Light() noexcept = default;
	~Light() noexcept override = default;

	[[nodiscard]] float GetIntensity() const noexcept;
	void SetIntensity(float intensity_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

	void OnEnable() override;
	void OnDisable() override;

private:
	float intensity{ 1.0f };
	ColorRGBA color{ ColorRGBA::GetWhite() };
};
