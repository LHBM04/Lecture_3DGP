#pragma once

#include <d3d12.h>

#include <wrl.h>

#include "Framework/Graphics/GraphicsDevice.h"

namespace TUK::Framework
{
	class GraphicsDeviceInternal : public GraphicsDevice
	{
	public:
		explicit GraphicsDeviceInternal(Microsoft::WRL::ComPtr<ID3D12Device> device) noexcept;
		~GraphicsDeviceInternal() noexcept;

		GraphicsDeviceInternal(const GraphicsDeviceInternal&) = delete;
		GraphicsDeviceInternal& operator=(const GraphicsDeviceInternal&) = delete;

		GraphicsDeviceInternal(GraphicsDeviceInternal&&) = delete;
		GraphicsDeviceInternal& operator=(GraphicsDeviceInternal&&) = delete;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> device;
	};
}