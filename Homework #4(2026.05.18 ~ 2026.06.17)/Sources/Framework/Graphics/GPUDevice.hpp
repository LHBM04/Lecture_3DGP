#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class GPUDevice
	{
	public:
		explicit GPUDevice(ID3D12Device* d3dDevice) noexcept;
		~GPUDevice() noexcept;

	private:
		ID3D12Device* d3dDevice;
	};
}
