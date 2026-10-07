#include "Precompiled.h"
#include "SwapChain.h"

#include "../Core/Engine.h"
#include "../Platform/Window.h"
#include "GraphicsError.h"

namespace TUK::Framework
{
	SwapChain::SwapChain(Window& targetWindow) noexcept
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

	SwapChain::SwapChain(SwapChain&& other) noexcept
		: window(other.window)
		, windowHandle(other.windowHandle)
		, device(std::move(other.device))
		, swapChain(std::move(other.swapChain))
		, renderTargetHeap(std::move(other.renderTargetHeap))
		, buffers(std::move(other.buffers))
		, descriptorSize(other.descriptorSize)
		, positionX(other.positionX)
		, positionY(other.positionY)
		, sizeX(other.sizeX)
		, sizeY(other.sizeY)
	{
	}

	SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
	{
		if (this != &other)
		{
			device = std::move(other.device);
			swapChain = std::move(other.swapChain);
			renderTargetHeap = std::move(other.renderTargetHeap);
			buffers = std::move(other.buffers);
			window = other.window;
			windowHandle = other.windowHandle;
			descriptorSize = other.descriptorSize;
			positionX = other.positionX;
			positionY = other.positionY;
			sizeX = other.sizeX;
			sizeY = other.sizeY;
		}
		return *this;
	}

	std::expected<void, std::string> SwapChain::Initialize(ID3D12Device& renderDevice, IDXGIFactory6& factory,
		ID3D12CommandQueue& queue, UINT targetWidth, UINT targetHeight)
	{
		assert(windowHandle);
		const auto configuredBufferCount = Engine::GetInstance().GetOption<int>("RenderContext.BufferCount");
		if (configuredBufferCount < 2 || configuredBufferCount > 16)
		{
			return std::unexpected(std::string{ "RenderContext.BufferCount는 2 이상 16 이하여야 합니다." });
		}

		const UINT bufferCount = static_cast<UINT>(configuredBufferCount);

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
			return CheckHResult(result, "스왑 체인 생성");
		}
		result = createdSwapChain.As(&swapChain);
		if (FAILED(result))
		{
			return CheckHResult(result, "스왑 체인 인터페이스 조회");
		}
		result = factory.MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER);
		if (FAILED(result))
		{
			return CheckHResult(result, "창 연결 설정");
		}

		D3D12_DESCRIPTOR_HEAP_DESC heapDescription{};
		heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		heapDescription.NumDescriptors = bufferCount;
		result = renderDevice.CreateDescriptorHeap(&heapDescription,
			IID_PPV_ARGS(renderTargetHeap.GetAddressOf()));
		if (FAILED(result))
		{
			return CheckHResult(result, "RTV 힙 생성");
		}
		sizeX = targetWidth;
		sizeY = targetHeight;
		buffers.resize(bufferCount);
		return CreateRenderTargets();
	}

	Window& SwapChain::GetWindow() const noexcept
	{
		return window.get();
	}

	HWND SwapChain::GetHWND() const noexcept
	{
		return windowHandle;
	}

	std::expected<void, std::string> SwapChain::CreateRenderTargets()
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		auto descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT index = 0; index < buffers.size(); ++index)
		{
			const HRESULT result = swapChain->GetBuffer(index, IID_PPV_ARGS(buffers[index].GetAddressOf()));
			if (FAILED(result))
			{
				return CheckHResult(result, "백 버퍼 조회");
			}
			device->CreateRenderTargetView(buffers[index].Get(), nullptr, descriptor);
			descriptor.ptr += descriptorSize;
		}
		return {};
	}

	void SwapChain::Resize(UINT targetWidth, UINT targetHeight)
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		assert(targetWidth > 0 && targetHeight > 0);

		for (auto& buffer : buffers)
		{
			buffer.Reset();
		}

		const auto result = CheckHResult(
			swapChain->ResizeBuffers(static_cast<UINT>(buffers.size()), targetWidth, targetHeight,
				DXGI_FORMAT_R8G8B8A8_UNORM, 0),
			"스왑 체인 크기 변경");
		if (!result)
		{
			Engine::GetInstance().ReportError(result.error());
			return;
		}

		auto descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT index = 0; index < buffers.size(); ++index)
		{
			if (FAILED(swapChain->GetBuffer(index, IID_PPV_ARGS(buffers[index].GetAddressOf()))))
			{
				// return CheckHResult(result, "백 버퍼 조회");
				return;
			}

			device->CreateRenderTargetView(buffers[index].Get(), nullptr, descriptor);
			descriptor.ptr += descriptorSize;
		}

		sizeX = targetWidth;
		sizeY = targetHeight;
	}

	void SwapChain::Clear(ID3D12GraphicsCommandList& commandList, const std::array<float, 4>& color)
	{
		const UINT index = GetCurrentBufferIndex();

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = buffers[index].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList.ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE descriptor = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
		descriptor.ptr += static_cast<SIZE_T>(index) * descriptorSize;
		commandList.ClearRenderTargetView(descriptor, color.data(), 0, nullptr);
	}

	void SwapChain::Bind(ID3D12GraphicsCommandList& commandList) const
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

	void SwapChain::EndRender(ID3D12GraphicsCommandList& commandList) const
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

	std::expected<void, std::string> SwapChain::Present(UINT syncInterval)
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		return CheckHResult(swapChain->Present(syncInterval, 0), "프레임 표시");
	}

	UINT SwapChain::GetCurrentBufferIndex() const noexcept
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		return swapChain->GetCurrentBackBufferIndex();
	}

	int SwapChain::GetPositionX() const noexcept
	{
		return positionX;
	}

	void SwapChain::SetPositionX(int targetX) noexcept
	{
		positionX = targetX;
	}

	int SwapChain::GetPositionY() const noexcept
	{
		return positionY;
	}

	void SwapChain::SetPositionY(int targetY) noexcept
	{
		positionY = targetY;
	}

	UINT SwapChain::GetSizeX() const noexcept
	{
		return sizeX;
	}

	void SwapChain::SetSizeX(UINT targetWidth)
	{
		Resize(targetWidth, sizeY);
	}

	UINT SwapChain::GetSizeY() const noexcept
	{
		return sizeY;
	}

	void SwapChain::SetSizeY(UINT targetHeight)
	{
		Resize(sizeX, targetHeight);
	}
}
