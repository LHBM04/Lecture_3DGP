#include "Precompiled.h"
#include "RectTransform.h"

const Vector2D& RectTransform::GetAnchoredPosition() const noexcept
{
	return anchoredPosition;
}

void RectTransform::SetAnchoredPosition(const Vector2D& anchoredPosition_) noexcept
{
	anchoredPosition = anchoredPosition_;
	SetLocalPosition(Vector3D(anchoredPosition.x, anchoredPosition.y, 0.0f));
}

const Vector2D& RectTransform::GetSizeDelta() const noexcept
{
	return sizeDelta;
}

void RectTransform::SetSizeDelta(const Vector2D& sizeDelta_) noexcept
{
	sizeDelta.x = std::max(0.0f, sizeDelta_.x);
	sizeDelta.y = std::max(0.0f, sizeDelta_.y);
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

float RectTransform::GetLeft(float viewportWidth_) const noexcept
{
	return viewportWidth_ * 0.5f + anchoredPosition.x - sizeDelta.x * pivot.x;
}

float RectTransform::GetTop(float viewportHeight_) const noexcept
{
	return viewportHeight_ * 0.5f - anchoredPosition.y - sizeDelta.y * (1.0f - pivot.y);
}

float RectTransform::GetWidth() const noexcept
{
	return sizeDelta.x;
}

float RectTransform::GetHeight() const noexcept
{
	return sizeDelta.y;
}
