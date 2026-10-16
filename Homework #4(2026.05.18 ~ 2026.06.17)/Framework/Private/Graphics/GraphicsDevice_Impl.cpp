#include "Precompiled.hpp"
#include "GraphicsDevice_Impl.hpp"

namespace TUK::Framework
{
	GraphicsDevice_Impl::GraphicsDevice_Impl(ID3D12Device10* d3dDevice) noexcept
		: d3dDevice(d3dDevice)
	{
		assert(d3dDevice);
	}

	GraphicsDevice_Impl::~GraphicsDevice_Impl() noexcept
	{
		if (d3dDevice)
		{
			d3dDevice->Release();
			d3dDevice = nullptr;
		}
	}

	ID3D12Device10& GraphicsDevice_Impl::GetD3D12Device() const noexcept
	{
		assert(d3dDevice);
		return *d3dDevice;
	}
}
