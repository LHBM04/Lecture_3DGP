#pragma once

#include "Component.hpp"
#include "Matrix4x4.hpp"

class Camera : public Component
{
public:
	struct Viewport final
	{
		float x;
		float y;
		float width;
		float height;
	};

	[[nodiscard]] Viewport GetViewport() const;
	void SetViewport(const Viewport& viewport_);
	void SetViewport(float x_, float y_, float width_, float height_);

	[[nodiscard]] float GetNearClipPlane() const;
	void SetNearClipPlane(float nearClipPlane_);

	[[nodiscard]] float GetFarClipPlane() const;
	void SetFarClipPlane(float farClipPlane_);

	[[nodiscard]] Matrix4x4 GetViewMatrix() const;
	[[nodiscard]] Matrix4x4 GetViewProjectionMatrix(float aspectRatio_) const;

	[[nodiscard]] virtual Matrix4x4 GetProjectionMatrix(float aspectRatio_) const = 0;

	virtual ~Camera() = default;

protected:
	void OnAttach() override;
	void OnDetach() override;

private:
	Viewport viewport{ 0.0f, 0.0f, 1.0f, 1.0f };
	float nearClipPlane{ 0.1f };
	float farClipPlane{ 100.0f };
};

