#include "Precompiled.h"
#include "RectTransform.h"

#include "InputManager.h"
#include "TimeContext.h"

const Vector2D& RectTransform::GetAnchoredPosition() const noexcept
{
	return anchoredPosition;
}

void RectTransform::SetAnchoredPosition(const Vector2D& anchoredPosition_) noexcept
{
	anchoredPosition = anchoredPosition_;
}

const Vector2D& RectTransform::GetSize() const noexcept
{
	return size;
}

void RectTransform::SetSize(const Vector2D& size_) noexcept
{
	size.x = std::max(1.0f, size_.x);
	size.y = std::max(1.0f, size_.y);
}

const Vector2D& RectTransform::GetPivot() const noexcept
{
	return pivot;
}

void RectTransform::SetPivot(const Vector2D& pivot_) noexcept
{
	pivot.x = Mathf::Clamp(pivot_.x, 0.0f, 1.0f);
	pivot.y = Mathf::Clamp(pivot_.y, 0.0f, 1.0f);
}

bool RectTransform::ContainsScreenPoint(int x_, int y_) const noexcept
{
	const auto [screenWidth, screenHeight]{ InputManager::GetScreenSize() };

	const float centerX{ screenWidth * 0.5f + anchoredPosition.x };
	const float centerY{ screenHeight * 0.5f - anchoredPosition.y };

	const float minX{ centerX - size.x * pivot.x };
	const float minY{ centerY - size.y * pivot.y };
	const float maxX{ minX + size.x };
	const float maxY{ minY + size.y };

	return x_ >= static_cast<int>(minX) &&
		x_ <= static_cast<int>(maxX) &&
		y_ >= static_cast<int>(minY) &&
		y_ <= static_cast<int>(maxY);
}

void RectTransform::OnUpdate(const TimeContext& context_)
{
	(void)context_;
}
