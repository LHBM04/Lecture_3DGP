#include "Precompiled.h"
#include "RenderDevice.h"

bool RenderDevice::Initialize()
{
	UINT factoryFlags{ 0 };

	if (FAILED(::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(factory.GetAddressOf()))))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex{ 0 };
		factory->EnumAdapters1(adapterIndex, adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND;
		++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc{};
		adapter->GetDesc1(&desc);
		if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0)
		{
			continue;
		}

		if (SUCCEEDED(::D3D12CreateDevice(
			adapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(device.GetAddressOf()))))
		{
			break;
		}
	}

	if (device == nullptr)
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
		if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(warpAdapter.GetAddressOf()))))
		{
			return false;
		}

		if (FAILED(::D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(device.GetAddressOf()))))
		{
			return false;
		}
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels{};
	qualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	qualityLevels.SampleCount = 4;
	qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	qualityLevels.NumQualityLevels = 0;

	if (SUCCEEDED(device->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&qualityLevels,
		sizeof(qualityLevels))))
	{
		msaa4xQualityLevels = qualityLevels.NumQualityLevels;
		isEnableMsaa4x = msaa4xQualityLevels > 0;
	}

	return true;
}

void RenderDevice::Release()
{
	device.Reset();
	factory.Reset();
	isEnableMsaa4x = false;
	msaa4xQualityLevels = 0;
}

ID3D12Device& RenderDevice::GetDevice() const noexcept
{
	assert(device != nullptr);
	return *device.Get();
}

IDXGIFactory4& RenderDevice::GetFactory() const noexcept
{
	assert(factory != nullptr);
	return *factory.Get();
}
