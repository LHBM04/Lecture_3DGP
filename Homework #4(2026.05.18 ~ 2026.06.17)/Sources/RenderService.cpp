#include "Precompiled.h"
#include "RenderService.h"

bool RenderService::Initialize(const Options& options_)
{
    options = options_;

	UINT factoryFlags{ 0 };

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(::D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		debugController->EnableDebugLayer();
	}
#endif

    if (HRESULT result{ ::CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)) }; FAILED(result))
	{
        std::string errorMessage{ std::system_category().message(result) };
        std::cout << "Error Message: " << errorMessage << std::endl;

        return false;
	}

	if (HRESULT result{ CreateDevice() }; FAILED(result))
	{
		std::string errorMessage{ std::system_category().message(result) };
		std::cout << "Error Message: " << errorMessage << std::endl;

		return false;
	}

	return true;
}

void RenderService::Terminate() noexcept
{

}

ID3D12Device* RenderService::GetD3DDevice() const noexcept
{
	return device.Get();
}

void RenderService::OnAdd()
{

}

void RenderService::OnRemove()
{

}

HRESULT RenderService::CreateDevice()
{
	assert(factory != nullptr);

    HRESULT result{ S_OK };
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

    for (std::size_t count{ 0 }; factory->EnumAdapters1(static_cast<UINT>(count), &adapter) != DXGI_ERROR_NOT_FOUND; count++)
    {
        DXGI_ADAPTER_DESC1 adapterDescription;
        adapter->GetDesc1(&adapterDescription);

        if (adapterDescription.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        result = ::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
        if (SUCCEEDED(result))
        {
            return {};
        }
    }

    if (device == nullptr)
    {
        if (SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter))))
        {
            result = ::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
            if (SUCCEEDED(result))
            {
                return {};
            }
        }
    }

    return result;
}
