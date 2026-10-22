#pragma once

#include "Framework/Math/Vector3D.hpp"

namespace TUK::Framework
{
	struct Vertex final
	{
		Vector3D<float> position;
		Vector3D<float> normal;
	};
}
