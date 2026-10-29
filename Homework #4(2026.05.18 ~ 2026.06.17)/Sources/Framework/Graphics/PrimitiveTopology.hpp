#pragma once

#include <cstdint>
#include <utility>

#include <d3d12.h>

namespace TUK::Framework
{
	enum class PrimitiveTopology : std::uint8_t
	{
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip
	};

	[[nodiscard]] inline D3D12_PRIMITIVE_TOPOLOGY ToD3D12PrimitiveTopology(PrimitiveTopology topology) noexcept
	{
		using enum D3D12_PRIMITIVE_TOPOLOGY;

		switch (topology) 
		{
			case PrimitiveTopology::PointList:     return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
			case PrimitiveTopology::LineList:      return D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			case PrimitiveTopology::LineStrip:     return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
			case PrimitiveTopology::TriangleList:  return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			case PrimitiveTopology::TriangleStrip: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		}

		std::unreachable();
	}
}
