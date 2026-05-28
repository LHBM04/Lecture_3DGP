#include "Precompiled.h"
#include "RenderSystem.h"

bool RenderSystem::Initialize()
{
	UINT factoryFlags{ 0 };
#if defined(_DEBUG)
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
	{
		debugController->EnableDebugLayer();
		factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
	{
		return false;
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter{ nullptr };
	if (FAILED(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
	{
		return false;
	}

	if (FAILED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	if (FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue))))
	{
		return false;
	}

	if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
	{
		return false;
	}

	fenceEvent = ::CreateEventExW(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
	fenceValue = 1;

	return true;
}

void RenderSystem::Release()
{
	fence.Reset();
	commandQueue.Reset();
	device.Reset();
	factory.Reset();

	if (nullptr != fenceEvent)
	{
		::CloseHandle(fenceEvent);
		fenceEvent = nullptr;
	}

#if defined(_DEBUG)
	debugController.Reset();
#endif

}

std::expected<Renderer*, std::wstring> RenderSystem::CreateRenderer(const RendererOptions& options_)
{
	return std::unexpected<std::wstring>(L"Not implemented");
}

void RenderSystem::DestroyRenderer(Renderer* renderer_)
{

}
