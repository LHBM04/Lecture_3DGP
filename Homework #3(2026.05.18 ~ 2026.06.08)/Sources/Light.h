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
	~Light() noexcept = default;
};
