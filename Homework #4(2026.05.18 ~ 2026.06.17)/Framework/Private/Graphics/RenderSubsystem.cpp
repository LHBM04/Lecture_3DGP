#include "Precompiled.hpp"
#include "Framework/Graphics/RenderSubsystem.hpp"

#include "Graphics/CommandBuffer_Impl.hpp"
#include "Graphics/Fence_Impl.hpp"
#include "Graphics/GraphicsDevice_Impl.hpp"
#include "Graphics/SwapChain_Impl.hpp"
#include "Platform/Display_Impl.hpp"

namespace
{
	/** DXGI Factory */
	IDXGIFactory6* dxgiFactory = nullptr;

	/** D3D12 명령 큐 */
	ID3D12CommandQueue* d3dCommandQueue = nullptr;
}

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
		, device(nullptr)
		, commandBuffer(nullptr)
		, fence(nullptr)
	{
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		if (d3dCommandQueue && fence)
		{
			Fence_Impl& fenceImpl = dynamic_cast<Fence_Impl&>(*fence);
			fenceImpl.Signal(*d3dCommandQueue);
			fenceImpl.Wait();
		}

		swapChains.clear();
		commandBuffer.reset();
		fence.reset();

		if (d3dCommandQueue)
		{
			d3dCommandQueue->Release();
			d3dCommandQueue = nullptr;
		}

		device.reset();

		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = nullptr;
		}
	}

	void RenderSubsystem::CreateSwapChain(const Display& window)
	{
		if (swapChains.contains(&window))
		{
			return;
		}

		assert(dxgiFactory && d3dCommandQueue);

		const HWND hWnd = dynamic_cast<const Display_Impl&>(window).GetHWND();
		assert(hWnd);

		const Vector2D<std::int32_t> clientSize = window.GetClientSize();
		const UINT width = static_cast<UINT>((std::max)(1, clientSize.GetX()));
		const UINT height = static_cast<UINT>((std::max)(1, clientSize.GetY()));

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = width;
		description.Height = height;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.SampleDesc.Count = 1;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.BufferCount = 2;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		IDXGISwapChain1* createdSwapChain = nullptr;
		if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
			d3dCommandQueue,
			hWnd,
			&description,
			nullptr,
			nullptr,
			&createdSwapChain)))
		{
			return;
		}

		dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		IDXGISwapChain4* dxgiSwapChain = nullptr;
		const HRESULT hr = createdSwapChain->QueryInterface(IID_PPV_ARGS(&dxgiSwapChain));
		createdSwapChain->Release();
		if (FAILED(hr) || !dxgiSwapChain)
		{
			return;
		}

		swapChains.emplace(&window, std::make_unique<SwapChain_Impl>(dxgiSwapChain));
	}

	SystemStatus RenderSubsystem::OnStartup()
	{
		UINT factoryFlags = 0;

#ifdef _DEBUG
		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
			debugController->Release();
		}
#endif

		if (FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		{
			return SystemStatus::Failed;
		}

		ID3D12Device10* d3dDevice = nullptr;
		for (UINT index = 0; ; ++index)
		{
			IDXGIAdapter1* adapter = nullptr;
			if (dxgiFactory->EnumAdapterByGpuPreference(
				index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&adapter)) == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}

			DXGI_ADAPTER_DESC1 adapterDesc{};
			adapter->GetDesc1(&adapterDesc);
			if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapter->Release();
				continue;
			}

			const HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
			adapter->Release();
			if (SUCCEEDED(hr))
			{
				break;
			}
		}

		if (!d3dDevice)
		{
			IDXGIAdapter* warpAdapter = nullptr;
			if (FAILED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
			{
				return SystemStatus::Failed;
			}

			const HRESULT hr = D3D12CreateDevice(warpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
			warpAdapter->Release();
			if (FAILED(hr))
			{
				return SystemStatus::Failed;
			}
		}

		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dCommandQueue))))
		{
			d3dDevice->Release();
			return SystemStatus::Failed;
		}

		ID3D12CommandAllocator* d3dCommandAllocator = nullptr;
		if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3dCommandAllocator))))
		{
			d3dDevice->Release();
			return SystemStatus::Failed;
		}

		ID3D12GraphicsCommandList* d3dCommandList = nullptr;
		if (FAILED(d3dDevice->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator,
			nullptr,
			IID_PPV_ARGS(&d3dCommandList))))
		{
			d3dCommandAllocator->Release();
			d3dDevice->Release();
			return SystemStatus::Failed;
		}

		d3dCommandList->Close();

		ID3D12Fence* d3dFence = nullptr;
		if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3dFence))))
		{
			d3dCommandList->Release();
			d3dCommandAllocator->Release();
			d3dDevice->Release();
			return SystemStatus::Failed;
		}

		const HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!fenceEvent)
		{
			d3dFence->Release();
			d3dCommandList->Release();
			d3dCommandAllocator->Release();
			d3dDevice->Release();
			return SystemStatus::Failed;
		}

		device = std::make_unique<GraphicsDevice_Impl>(d3dDevice);
		commandBuffer = std::make_unique<CommandBuffer_Impl>(d3dCommandAllocator, d3dCommandList);
		fence = std::make_unique<Fence_Impl>(d3dFence, fenceEvent);

		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		assert(commandBuffer);

		commandBuffer->BeginFrame();
		for (const auto& swapChain : swapChains | std::views::values)
		{
			swapChain->Clear(*commandBuffer);
		}

		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		assert(commandBuffer && d3dCommandQueue && fence);

		commandBuffer->EndFrame();

		ID3D12CommandList* commandLists[]{ &dynamic_cast<CommandBuffer_Impl&>(*commandBuffer).GetD3D12CommandList() };
		d3dCommandQueue->ExecuteCommandLists(1, commandLists);

		for (const auto& swapChain : swapChains | std::views::values)
		{
			swapChain->Present();
		}

		Fence_Impl& fenceImpl = dynamic_cast<Fence_Impl&>(*fence);
		fenceImpl.Signal(*d3dCommandQueue);
		fenceImpl.Wait();

		return SystemStatus::Continue;
	}

	void RenderSubsystem::OnShutdown()
	{
		if (d3dCommandQueue && fence)
		{
			Fence_Impl& fenceImpl = dynamic_cast<Fence_Impl&>(*fence);
			fenceImpl.Signal(*d3dCommandQueue);
			fenceImpl.Wait();
		}

		swapChains.clear();
		commandBuffer.reset();
		fence.reset();

		if (d3dCommandQueue)
		{
			d3dCommandQueue->Release();
			d3dCommandQueue = nullptr;
		}

		device.reset();

		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = nullptr;
		}
	}
}
