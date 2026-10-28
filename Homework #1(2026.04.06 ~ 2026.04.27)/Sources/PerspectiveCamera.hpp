#pragma once

#include "Camera.hpp"

class PerspectiveCamera final : public Camera
{
public:
	[[nodiscard]] float GetFieldOfView() const;
	void SetFieldOfView(float fieldOfViewDegrees_);

	[[nodiscard]] Matrix4x4 GetProjectionMatrix(float aspectRatio_) const override;

private:
	float fieldOfViewDegrees{ 60.0f };
};

