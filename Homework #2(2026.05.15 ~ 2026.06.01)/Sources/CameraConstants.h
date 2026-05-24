#pragma once

#include "Matrix4x4.h"

struct CameraConstants final
{
	Matrix4x4 view;
	Matrix4x4 projection;
	Matrix4x4 viewProjection;
};
