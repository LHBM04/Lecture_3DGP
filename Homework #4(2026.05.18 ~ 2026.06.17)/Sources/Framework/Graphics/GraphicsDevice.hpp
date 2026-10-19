#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class GraphicsDevice
	{
	public:
		explicit GraphicsDevice(ID3D12Device10* d3dDevice) noexcept;
		~GraphicsDevice() noexcept;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;

		GraphicsDevice(GraphicsDevice&&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

		/** D3D12 Device Getter */
		[[nodiscard]] ID3D12Device10& GetD3D12Device() const noexcept;

	private:
		ID3D12Device10* d3dDevice;
	};
}
