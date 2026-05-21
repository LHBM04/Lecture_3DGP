#pragma once

#include "Transform.h"
#include "Vector2D.h"

class Renderer;

class RectTransform final : public Transform
{
public:
	[[nodiscard]] const Vector2D& GetAnchoredPosition() const noexcept;
	void SetAnchoredPosition(const Vector2D& anchoredPosition_) noexcept;

	[[nodiscard]] const Vector2D& GetSizeDelta() const noexcept;
	void SetSizeDelta(const Vector2D& sizeDelta_) noexcept;

	[[nodiscard]] const Vector2D& GetPivot() const noexcept;
	void SetPivot(const Vector2D& pivot_) noexcept;

	[[nodiscard]] float GetLeft(float viewportWidth_) const noexcept;
	[[nodiscard]] float GetTop(float viewportHeight_) const noexcept;
	[[nodiscard]] float GetWidth() const noexcept;
	[[nodiscard]] float GetHeight() const noexcept;

private:
	Vector2D anchoredPosition{ Vector2D::GetZero() };
	Vector2D sizeDelta{ 100.0f, 100.0f };
	Vector2D pivot{ 0.5f, 0.5f };
};
