#include "Precompiled.h"
#include "GraphicsDeviceInternal.h"

namespace TUK::Framework
{
	GraphicsDeviceInternal::GraphicsDeviceInternal(Microsoft::WRL::ComPtr<ID3D12Device> device) noexcept
		: device(std::move(device))
	{
		
	}

	GraphicsDeviceInternal::~GraphicsDeviceInternal() noexcept
	{
	}
}
