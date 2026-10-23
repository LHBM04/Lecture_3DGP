#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class GPUDevice
	{
	public:
		explicit GPUDevice(ID3D12Device* d3dDevice) noexcept;
		~GPUDevice() noexcept;

		GPUDevice(const GPUDevice&) = delete;
		GPUDevice& operator=(const GPUDevice&) = delete;

		GPUDevice(GPUDevice&&) noexcept = delete;
		GPUDevice& operator=(GPUDevice&&) noexcept = delete;

		[[nodiscard]] ID3D12Device& GetD3DDevice() const noexcept;

	private:
		ID3D12Device* d3dDevice;
	};
}
