#pragma once

#include "ColorRGB.h"
#include "Component.h"
#include "Vector3D.h"

class Light final : public Component
{
public:
	enum class Type : unsigned char
	{
		Directional,
		Point
	};

	~Light() override = default;

	[[nodiscard]] Type GetType() const noexcept;
	void SetType(Type type_) noexcept;

	[[nodiscard]] const ColorRGB& GetColor() const noexcept;
	void SetColor(const ColorRGB& color_) noexcept;

	[[nodiscard]] float GetIntensity() const noexcept;
	void SetIntensity(float intensity_) noexcept;

	[[nodiscard]] const Vector3D& GetDirection() const noexcept;
	void SetDirection(const Vector3D& direction_) noexcept;

private:
	Type type{ Type::Directional };
	ColorRGB color{ ColorRGB::GetWhite() };
	float intensity{ 1.0f };
	Vector3D direction{ Vector3D::GetForward() };
};
