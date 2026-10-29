#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>

namespace TUK::Framework
{
	struct Viewport final
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;

		[[nodiscard]] operator D3D12_VIEWPORT() const noexcept;
	};

	inline Viewport::operator D3D12_VIEWPORT() const noexcept
	{
		return D3D12_VIEWPORT{ x, y, width, height, minDepth, maxDepth };
	}
}
