#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "Vector4D.h"

struct alignas(256) LightConstants final
{
	Vector4D lightDirection;
	ColorRGBA lightColor;
};

class Light : public Component
{
public:
	Light() noexcept = default;
	~Light() noexcept override = default;

	[[nodiscard]] Vector4D GetLightDirection() const noexcept;
	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

protected:
	void OnEnable() override;
	void OnDisable() override;

private:
	ColorRGBA color{ ColorRGBA::GetWhite() };
};
