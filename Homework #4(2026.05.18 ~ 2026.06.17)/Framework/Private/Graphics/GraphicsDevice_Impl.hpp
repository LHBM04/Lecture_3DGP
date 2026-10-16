#pragma once

#include <d3d12.h>

#include "Framework/Graphics/GraphicsDevice.hpp"

namespace TUK::Framework
{
	class GraphicsDevice_Impl : public GraphicsDevice
	{
	public:
		explicit GraphicsDevice_Impl(ID3D12Device10* d3dDevice) noexcept;
		~GraphicsDevice_Impl() noexcept override;

		/** D3D12 디바이스 Getter */
		[[nodiscard]] ID3D12Device10& GetD3D12Device() const noexcept;

	private:
		ID3D12Device10* d3dDevice;
	};
}
