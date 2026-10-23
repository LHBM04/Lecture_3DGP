#include "Precompiled.hpp"
#include "RenderSubsystem.hpp"

#include "../Platform/Display.hpp"

#include "CommandBuffer.hpp"
#include "Fence.hpp"
#include "RenderPassInfo.hpp"
#include "../../../../Homework #3(2026.05.18 ~ 2026.06.08)/Sources/Logger.h"

namespace
{
	/** DXGI Factory */
	IDXGIFactory6* dxgiFactory = nullptr;

	std::expected<ID3D12RootSignature*, std::string> CreateRootSignature(ID3D12Device10& device)
	{
		D3D12_ROOT_SIGNATURE_DESC description{};
		description.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		if (FAILED(D3D12SerializeRootSignature(
			&description,
			D3D_ROOT_SIGNATURE_VERSION_1,
			signatureBlob.GetAddressOf(),
			errorBlob.GetAddressOf())))
		{
			if (errorBlob)
			{
				return std::unexpected(std::string(
					static_cast<const char*>(errorBlob->GetBufferPointer()),
					errorBlob->GetBufferSize()));
			}
			return std::unexpected("Failed to serialize root signature");
		}

		ID3D12RootSignature* rootSignature = nullptr;
		if (FAILED(device.CreateRootSignature(
			0,
			signatureBlob->GetBufferPointer(),
			signatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&rootSignature))))
		{
			return std::unexpected("Failed to create root signature");
		}

		return rootSignature;
	}
}

namespace TUK::Framework
{
	RenderSubsystem::RenderSubsystem() noexcept
		: EngineSubsystem(100)
		, device(std::nullopt)
	{
	}

	RenderSubsystem::~RenderSubsystem() noexcept
	{
		
	}

	void RenderSubsystem::RegisterDisplay(const Display& window)
	{
		if (swapChains.contains(&window))
		{
			return;
		}

		assert(dxgiFactory && device.GetD3D12CommandQueue());

		const HWND hWnd = window.GetHWND();
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
			device.value().GetD3D12CommandQueue(),
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

		swapChains.try_emplace(&window, dxgiSwapChain);
	}

	void RenderSubsystem::UnregisterDisplay(const Display& window)
	{
		if (activeCommandBuffer)
		{
			pendingDisplayRemovals.push_back(&window);
			return;
		}

		renderCallbacks.erase(&window);
		swapChains.erase(&window);
	}

	void RenderSubsystem::SetRenderCallback(const Display& window, std::function<void(CommandBuffer&)> callback)
	{
		SetRenderCallbacks(window, RenderCallbacks{ .render = std::move(callback) });
	}

	void RenderSubsystem::SetRenderCallbacks(const Display& window, RenderCallbacks callbacks)
	{
		renderCallbacks.insert_or_assign(&window, std::move(callbacks));
	}

	void RenderSubsystem::RemovePendingDisplays()
	{
		for (const Display* window : pendingDisplayRemovals)
		{
			renderCallbacks.erase(window);
			swapChains.erase(window);
		}
		pendingDisplayRemovals.clear();
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
			return SystemStatus::Failure;
		}

		ID3D12Device10* d3dDevice = nullptr;
		for (UINT index = 0; ; ++index)
		{
			IDXGIAdapter1* adapter = nullptr;
			const HRESULT adapterResult = dxgiFactory->EnumAdapterByGpuPreference(
				index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&adapter));
			if (adapterResult == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}
			if (FAILED(adapterResult))
			{
				dxgiFactory->Release();
				dxgiFactory = nullptr;
				return SystemStatus::Failure;
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
				dxgiFactory->Release();
				dxgiFactory = nullptr;
				return SystemStatus::Failure;
			}

			const HRESULT hr = D3D12CreateDevice(warpAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice));
			warpAdapter->Release();
			if (FAILED(hr))
			{
				dxgiFactory->Release();
				dxgiFactory = nullptr;
				return SystemStatus::Failure;
			}
		}

		ID3D12CommandQueue* d3dCommandQueue = nullptr;
		D3D12_COMMAND_QUEUE_DESC queueDesc{};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dCommandQueue))))
		{
			d3dDevice->Release();
			dxgiFactory->Release();
			dxgiFactory = nullptr;
			return SystemStatus::Failure;
		}

		auto rootSignature = CreateRootSignature(*d3dDevice);
		if (!rootSignature.has_value())
		{
			d3dCommandQueue->Release();
			d3dDevice->Release();
			dxgiFactory->Release();
			dxgiFactory = nullptr;
			return SystemStatus::Failure;
		}

		device = GraphicsDevice(d3dDevice, d3dCommandQueue, *rootSignature);

		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPreTick()
	{
		assert(!activeCommandBuffer);
		if (swapChains.empty())
		{
			return SystemStatus::Continue;
		}

		auto expectedCommandBuffer = device.value().AcquireCommandBuffer();
		if (!expectedCommandBuffer.has_value())
		{
			return SystemStatus::Failure;
		}

		activeCommandBuffer = &expectedCommandBuffer->get();
		return SystemStatus::Continue;
	}

	SystemStatus RenderSubsystem::OnPostTick()
	{
		if (!activeCommandBuffer)
		{
			return SystemStatus::Continue;
		}

		auto expectedFence = device.value().AcquireFence();
		if (!expectedFence.has_value())
		{
			activeCommandBuffer->GetD3D12CommandList().Close();
			activeCommandBuffer = nullptr;
			RemovePendingDisplays();
			return SystemStatus::Failure;
		}

		std::vector<const Display*> displays;
		displays.reserve(swapChains.size());
		for (const auto& [window, swapChain] : swapChains)
		{
			displays.push_back(window);
		}

		std::vector<const Display*> renderedDisplays;
		renderedDisplays.reserve(displays.size());
		for (const Display* window : displays)
		{
			if (std::ranges::find(pendingDisplayRemovals, window) != pendingDisplayRemovals.end())
			{
				continue;
			}

			SwapChain& swapChain = swapChains.at(window);
			RenderCallbacks callbacks;
			if (auto result = renderCallbacks.find(window); result != renderCallbacks.end())
			{
				callbacks = result->second;
			}

			std::array renderTargets{ std::ref(swapChain.GetCurrentRenderTarget()) };
			RenderPassInfo renderPassInfo{};
			renderPassInfo.renderTargets = renderTargets;
			renderPassInfo.clearColor = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
			renderPassInfo.operationBefore = RenderPassBeginningOperation::Clear;

			bool isRenderPassActive = false;
			try
			{
				if (callbacks.preRender)
				{
					callbacks.preRender(*activeCommandBuffer);
				}
				if (std::ranges::find(pendingDisplayRemovals, window) != pendingDisplayRemovals.end())
				{
					continue;
				}

				activeCommandBuffer->BeginRenderPass(renderPassInfo);
				isRenderPassActive = true;
				const auto size = swapChain.GetSize();
				D3D12_VIEWPORT viewport{};
				viewport.Width = static_cast<float>(size.GetX());
				viewport.Height = static_cast<float>(size.GetY());
				viewport.MaxDepth = 1.0f;
				const D3D12_RECT scissorRect{ 0, 0, size.GetX(), size.GetY() };
				activeCommandBuffer->GetD3D12CommandList().RSSetViewports(1, &viewport);
				activeCommandBuffer->GetD3D12CommandList().RSSetScissorRects(1, &scissorRect);
				if (callbacks.render)
				{
					callbacks.render(*activeCommandBuffer);
				}
				activeCommandBuffer->EndRenderPass();
				isRenderPassActive = false;

				if (callbacks.postRender)
				{
					callbacks.postRender(*activeCommandBuffer);
				}
			}
			catch (...)
			{
				if (isRenderPassActive)
				{
					activeCommandBuffer->EndRenderPass();
				}
				activeCommandBuffer->GetD3D12CommandList().Close();
				activeCommandBuffer = nullptr;
				RemovePendingDisplays();
				throw;
			}
			renderedDisplays.push_back(window);
		}

		auto submitted = device.value().SubmitCommandBuffer(*activeCommandBuffer);
		activeCommandBuffer = nullptr;
		if (!submitted.has_value())
		{
			RemovePendingDisplays();
			return SystemStatus::Failure;
		}

		for (const Display* window : renderedDisplays)
		{
			if (std::ranges::find(pendingDisplayRemovals, window) == pendingDisplayRemovals.end())
			{
				swapChains.at(window).Present();
			}
		}

		Fence& fence = expectedFence->get();
		assert(device.value().GetD3D12CommandQueue());
		fence.Signal(*device.value().GetD3D12CommandQueue());
		fence.Wait();
		RemovePendingDisplays();

		return SystemStatus::Continue;
	}

	void RenderSubsystem::OnShutdown()
	{
		if (activeCommandBuffer)
		{
			activeCommandBuffer->GetD3D12CommandList().Close();
			activeCommandBuffer = nullptr;
		}
		renderCallbacks.clear();
		pendingDisplayRemovals.clear();

		if (device.has_value() && device.value().GetD3D12CommandQueue())
		{
			auto expectedFence = device.value().AcquireFence();
			if (expectedFence.has_value())
			{
				Fence& fence = expectedFence->get();
				fence.Signal(*device.value().GetD3D12CommandQueue());
				fence.Wait();
			}
		}

		activeCommandBuffer = nullptr;
		swapChains.clear();
		device = std::nullopt;

		if (dxgiFactory)
		{
			dxgiFactory->Release();
			dxgiFactory = nullptr;
		}
	}
}
