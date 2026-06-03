#pragma once

#include <DirectXMath.h>

class Material;
class Mesh;

using Matrix4x4 = DirectX::XMFLOAT4X4;

struct RenderRequest final
{
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
	Matrix4x4 transform{};
};
