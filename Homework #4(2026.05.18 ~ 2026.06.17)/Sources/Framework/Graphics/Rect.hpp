#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <d3d12.h>

namespace TUK::Framework
{
	struct Rect final
	{
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;

		[[nodiscard]] operator D3D12_RECT() const noexcept;
	};

	inline Rect::operator D3D12_RECT() const noexcept
	{
		return D3D12_RECT{ x, y, x + width, y + height };
	}
}
