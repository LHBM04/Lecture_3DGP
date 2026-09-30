#include "Precompiled.h"
#include "Framework/Graphics/RenderSubsystem.h"

#include "GraphicsDeviceInternal.h"

namespace
{
	using Microsoft::WRL::ComPtr;

	UINT ConfigureDebugLayer()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug> d3dDebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebugController))))
		{
			d3dDebugController->EnableDebugLayer();
		}

		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		return factoryFlags;
	}

	ComPtr<ID3D12Device> CreateHardwareDevice(IDXGIFactory4& factory)
	{
		for (UINT index = 0; ; ++index)
		{
			ComPtr<IDXGIAdapter1> adapter;
			const HRESULT result = factory.EnumAdapters1(index, adapter.GetAddressOf());
			if (FAILED(result))
			{
				return {}; // Includes DXGI_ERROR_NOT_FOUND when enumeration ends.
			}

			DXGI_ADAPTER_DESC1 dxgiAdapterDesc{};
			if (FAILED(adapter->GetDesc1(&dxgiAdapterDesc)))
			{
				continue;
			}
			if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			ComPtr<ID3D12Device> device;
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device))))
			{
				return device;
			}
		}
	}

	ComPtr<ID3D12Device> CreateWarpDevice(IDXGIFactory4& factory)
	{
		ComPtr<IDXGIAdapter1> adapter;
		if (FAILED(factory.EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
		{
			return {};
		}

		ComPtr<ID3D12Device> device;
		if (FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			return {};
		}
		return device;
	}
}

namespace TUK::Framework
{
	void RenderSubsystem::OnStartup()
	{
		const UINT factoryFlags = ConfigureDebugLayer();
		ComPtr<IDXGIFactory4> dxgiFactory;
		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			std::abort();
		}

		auto d3dDevice = CreateHardwareDevice(*dxgiFactory.Get());
		if (!d3dDevice)
		{
			d3dDevice = CreateWarpDevice(*dxgiFactory.Get());
		}

		if (!d3dDevice)
		{
			MessageBox(nullptr, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
			::PostQuitMessage(0);
			return;
		}

		device = std::make_unique<GraphicsDeviceInternal>(std::move(d3dDevice));
	}

	void RenderSubsystem::OnShutdown()
	{
		
	}

	GraphicsDevice& RenderSubsystem::GetDevice() const noexcept
	{
		ASSERT(device, L"GraphicsDevice가 nullptr입니다!");
		return *device;
	}
}
