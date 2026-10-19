#include "Precompiled.hpp"
#include "Framework/Graphics/GraphicsDevice.hpp"

namespace TUK::Framework
{
	GraphicsDevice::GraphicsDevice(ID3D12Device10* d3dDevice) noexcept
		: d3dDevice(d3dDevice)
	{
		assert(d3dDevice);
	}

	GraphicsDevice::~GraphicsDevice() noexcept
	{
		if (d3dDevice)
		{
			d3dDevice->Release();
			d3dDevice = nullptr;
		}
	}

	ID3D12Device10& GraphicsDevice::GetD3D12Device() const noexcept
	{
		assert(d3dDevice);
		return *d3dDevice;
	}
}
