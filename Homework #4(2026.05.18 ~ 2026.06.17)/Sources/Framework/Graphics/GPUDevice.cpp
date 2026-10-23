#include "Precompiled.hpp"
#include "GPUDevice.hpp"

namespace TUK::Framework
{
	GPUDevice::GPUDevice(ID3D12Device* d3dDevice) noexcept
		: d3dDevice(d3dDevice)
	{
	}

	GPUDevice::~GPUDevice() noexcept
	{
		// D3D12Device 해제
		if (d3dDevice)
		{
			d3dDevice->Release();
			d3dDevice = nullptr;
		}
	}
}
