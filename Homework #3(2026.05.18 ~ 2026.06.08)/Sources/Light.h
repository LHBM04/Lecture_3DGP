#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "Vector4D.h"

struct alignas(256) LightConstants final
{
	ColorRGBA ambientColor{ 0.1f, 0.1f, 0.1f, 1.0f };
	ColorRGBA lightColor{ ColorRGBA::GetWhite() };
	Vector4D lightDirection{ 0.0f, -1.0f, 0.0f, 0.0f };
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
	[[nodiscard]] LightConstants GetLightConstants() const noexcept;

	void OnEnable() override;
	void OnDisable() override;

private:
	float intensity{ 1.0f };
	ColorRGBA color{ ColorRGBA::GetWhite() };
};
