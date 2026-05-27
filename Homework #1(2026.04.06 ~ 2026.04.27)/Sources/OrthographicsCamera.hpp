#pragma once

#include "Camera.hpp"

class OrthographicsCamera final : public Camera
{
public:
	[[nodiscard]] float GetOrthographicSize() const;
	void SetOrthographicSize(float orthographicSize_);

	[[nodiscard]] Matrix4x4 GetProjectionMatrix(float aspectRatio_) const override;

private:
	float orthographicSize{ 5.0f };
};
