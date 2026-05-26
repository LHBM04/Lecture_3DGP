#pragma once

#include "Component.h"
#include "Updatable.h"
#include "Vector2D.h"

class RectTransform final : public Component, public Updatable
{
public:
	RectTransform() = default;
	~RectTransform() override = default;

	[[nodiscard]] const Vector2D& GetAnchoredPosition() const noexcept;
	void SetAnchoredPosition(const Vector2D& anchoredPosition_) noexcept;

	[[nodiscard]] const Vector2D& GetSize() const noexcept;
	void SetSize(const Vector2D& size_) noexcept;

	[[nodiscard]] const Vector2D& GetPivot() const noexcept;
	void SetPivot(const Vector2D& pivot_) noexcept;

	[[nodiscard]] bool ContainsScreenPoint(int x_, int y_) const noexcept;

protected:
	void OnUpdate() override;

private:
	Vector2D anchoredPosition{ 0.0f, 0.0f };
	Vector2D size{ 160.0f, 48.0f };
	Vector2D pivot{ 0.5f, 0.5f };
};
