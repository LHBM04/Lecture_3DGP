#include "Precompiled.h"
#include "RenderTarget.h"
#include "../Platform/Window.h"
#include "../Core/System.h"

#include <cassert>

namespace TUK::Framework
{
	RenderTarget::RenderTarget(Window& targetWindow) noexcept
		: window(targetWindow)
		, windowHandle(targetWindow.GetHWND())
		, device()
		, swapChain()
		, renderTargetHeap()
		, buffers()
		, descriptorSize(0)
		, positionX(0)
		, positionY(0)
		, sizeX(0)
		, sizeY(0)
	{
		assert(windowHandle);
	}

	HRESULT RenderTarget::Initialize(ID3D12Device& renderDevice, IDXGIFactory6& factory,
		ID3D12CommandQueue& queue, UINT targetWidth, UINT targetHeight)
	{
		assert(windowHandle);
		const auto option = System::GetInstance().GetOption<UINT>("Renderer.BufferCount");
		if (!option)
		{
			OutputDebugStringA(option.error().c_str());
			return E_INVALIDARG;
		}
		const UINT bufferCount = option->get();
		if (bufferCount < 2)
		{
			OutputDebugStringA("Renderer.BufferCount must be at least 2.\n");
			return E_INVALIDARG;
		}

		device = &renderDevice;
		const HWND handle = windowHandle;
		descriptorSize = renderDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		DXGI_SWAP_CHAIN_DESC1 description{};
		description.Width = targetWidth;
		description.Height = targetHeight;
		description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		description.SampleDesc.Count = 1;
		description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		description.BufferCount = bufferCount;
		description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> createdSwapChain;
		HRESULT result = factory.CreateSwapChainForHwnd(&queue, handle,
			&description, nullptr, nullptr, createdSwapChain.GetAddressOf());
		if (FAILED(result))
		{
			return result;
		}
		result = createdSwapChain.As(&swapChain);
		if (FAILED(result))
		{
			return result;
		}
		result = factory.MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(result))
		{
			return result;
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
		heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDescription.NumDescriptors = bufferCount;
		result = renderDevice.CreateDescriptorHeap(&heapDescription,
			IID_PPV_ARGS(renderTargetHeap.GetAddressOf()));
		if (FAILED(result))
		{
			return result;
		}
		sizeX = targetWidth;
		sizeY = targetHeight;
		buffers.resize(bufferCount);
		return CreateRenderTargets();
	}

	Window& RenderTarget::GetWindow() const noexcept
	{
		return window.get();
	}

	HWND RenderTarget::GetHWND() const noexcept
	{
		return windowHandle;
	}

	HRESULT RenderTarget::CreateRenderTargets()
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		auto descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT index = 0; index < buffers.size(); ++index)
		{
			const HRESULT result = swapChain->GetBuffer(index, IID_PPV_ARGS(buffers[index].GetAddressOf()));
			if (FAILED(result))
			{
				return result;
			}
			device->CreateRenderTargetView(buffers[index].Get(), nullptr, descriptor);
			descriptor.ptr += descriptorSize;
		}
		return S_OK;
	}

	HRESULT RenderTarget::Resize(UINT targetWidth, UINT targetHeight)
	{
		AssertInitialized();
		if (targetWidth == 0 || targetHeight == 0)
		{
			return E_INVALIDARG;
		}
		if (!swapChain)
		{
			return E_UNEXPECTED;
		}

		for (auto& buffer : buffers)
		{
			buffer.Reset();
		}
		const HRESULT result = swapChain->ResizeBuffers(static_cast<UINT>(buffers.size()), targetWidth, targetHeight,
			DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (FAILED(result))
		{
			return result;
		}
		sizeX = targetWidth;
		sizeY = targetHeight;
		return CreateRenderTargets();
	}

	void RenderTarget::Clear(ID3D12GraphicsCommandList& commandList, const std::array<float, 4>& color)
	{
		const UINT index = GetCurrentBufferIndex();
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = buffers[index].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList.ResourceBarrier(1, &barrier);

		auto descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		descriptor.ptr += static_cast<SIZE_T>(index) * descriptorSize;
		commandList.ClearRenderTargetView(descriptor, color.data(), 0, nullptr);
	}

	void RenderTarget::Bind(ID3D12GraphicsCommandList& commandList) const
	{
		const UINT index = GetCurrentBufferIndex();
		auto descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		descriptor.ptr += static_cast<SIZE_T>(index) * descriptorSize;
		commandList.OMSetRenderTargets(1, &descriptor, FALSE, nullptr);

		const D3D12_VIEWPORT viewport{
			static_cast<float>(positionX), static_cast<float>(positionY),
			static_cast<float>(sizeX), static_cast<float>(sizeY), 0.0f, 1.0f
		};
		const D3D12_RECT scissor{ 0, 0, static_cast<LONG>(sizeX), static_cast<LONG>(sizeY) };
		commandList.RSSetViewports(1, &viewport);
		commandList.RSSetScissorRects(1, &scissor);
	}

	void RenderTarget::EndRender(ID3D12GraphicsCommandList& commandList) const
	{
		const UINT index = GetCurrentBufferIndex();
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = buffers[index].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList.ResourceBarrier(1, &barrier);
	}

	HRESULT RenderTarget::Present(UINT syncInterval)
	{
		AssertInitialized();
		return swapChain->Present(syncInterval, 0);
	}

	void RenderTarget::AssertInitialized() const noexcept
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
	}

	UINT RenderTarget::GetCurrentBufferIndex() const noexcept
	{
		AssertInitialized();
		const UINT index = swapChain->GetCurrentBackBufferIndex();
		assert(buffers[index]);
		return index;
	}

	int RenderTarget::GetPositionX() const noexcept
	{
		return positionX;
	}

	void RenderTarget::SetPositionX(int targetX) noexcept
	{
		positionX = targetX;
	}

	int RenderTarget::GetPositionY() const noexcept
	{
		return positionY;
	}

	void RenderTarget::SetPositionY(int targetY) noexcept
	{
		positionY = targetY;
	}

	UINT RenderTarget::GetSizeX() const noexcept
	{
		return sizeX;
	}

	HRESULT RenderTarget::SetSizeX(UINT targetWidth)
	{
		return Resize(targetWidth, sizeY);
	}

	UINT RenderTarget::GetSizeY() const noexcept
	{
		return sizeY;
	}

	HRESULT RenderTarget::SetSizeY(UINT targetHeight)
	{
		return Resize(sizeX, targetHeight);
	}
}
