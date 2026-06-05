#pragma once

#include "ColorRGBA.h"
#include "Component.h"
#include "Vector3D.h"

class Light : public Component
{
public:
	enum class Type
	{
		Directional,
		Point,
		Spot
	};

	Light() noexcept = default;
	~Light() noexcept override = default;

	[[nodiscard]] Type GetType() const noexcept;
	void SetType(Type type_) noexcept;

	[[nodiscard]] const ColorRGBA& GetColor() const noexcept;
	void SetColor(const ColorRGBA& color_) noexcept;

	[[nodiscard]] float GetIntensity() const noexcept;
	void SetIntensity(float intensity_) noexcept;

	[[nodiscard]] float GetRange() const noexcept;
	void SetRange(float range_) noexcept;

	[[nodiscard]] float GetSpotAngle() const noexcept;
	void SetSpotAngle(float spotAngle_) noexcept;

	[[nodiscard]] Vector3D GetDirection() const noexcept;

protected:
	void OnEnable() override;
	void OnDisable() override;

private:
	Type type{ Type::Directional };
	ColorRGBA color{ ColorRGBA::GetWhite() };
	float intensity{ 1.0f };
	float range{ 10.0f };
	float spotAngle{ 30.0f };
};
