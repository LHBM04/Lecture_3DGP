#pragma once

#include "Vector3D.h"

struct LightConstants final
{
	Vector3D position;
	float range;
	Vector3D color;
	float intensity;
};
