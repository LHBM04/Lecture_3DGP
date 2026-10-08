#include "Precompiled.hpp"
#include "SwapChain.hpp"

#include "../Core/Engine.hpp"
#include "../Platform/Window.hpp"
#include "GraphicsError.hpp"

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

	SwapChain::~SwapChain() noexcept
	{
		Release();
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
			Release();
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
			return std::unexpected(std::string("RenderContext.BufferCount는 2 이상 16 이하여야 합니다."));
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
		const bool isExclusiveFullscreen = window.get().IsFullscreen() && !window.get().IsBorderless();
		description.Flags = isExclusiveFullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

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

		if (isExclusiveFullscreen)
		{
			Microsoft::WRL::ComPtr<IDXGIOutput> output;
			result = swapChain->GetContainingOutput(output.GetAddressOf());
			if (FAILED(result))
			{
				return CheckHResult(result, "전체 화면 출력 조회");
			}
			result = swapChain->SetFullscreenState(TRUE, output.Get());
			if (result != S_OK)
			{
				return std::unexpected(std::format("독점 전체 화면 전환 실패 (DXGI 결과: {:#010x}).",
					static_cast<unsigned long>(result)));
			}
			DXGI_MODE_DESC mode{};
			mode.Width = targetWidth;
			mode.Height = targetHeight;
			mode.Format = description.Format;
			result = swapChain->ResizeTarget(&mode);
			if (result != S_OK)
			{
				return std::unexpected(std::format("전체 화면 해상도 변경 실패 (DXGI 결과: {:#010x}).",
					static_cast<unsigned long>(result)));
			}
			result = swapChain->ResizeBuffers(bufferCount, 0, 0, description.Format, description.Flags);
			if (FAILED(result))
			{
				return CheckHResult(result, "전체 화면 백 버퍼 갱신");
			}
			result = swapChain->GetDesc1(&description);
			if (FAILED(result))
			{
				return CheckHResult(result, "전체 화면 백 버퍼 크기 조회");
			}
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
		sizeX = description.Width;
		sizeY = description.Height;
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

	void SwapChain::Release() noexcept
	{
		if (swapChain)
		{
			(void)swapChain->SetFullscreenState(FALSE, nullptr);
		}
		buffers.clear();
		renderTargetHeap.Reset();
		swapChain.Reset();
		device.Reset();
		descriptorSize = 0;
		sizeX = 0;
		sizeY = 0;
	}

	void SwapChain::Resize(UINT targetWidth, UINT targetHeight)
	{
		assert(device && swapChain && renderTargetHeap);
		assert(!buffers.empty());
		assert(targetWidth > 0 && targetHeight > 0);

		DXGI_SWAP_CHAIN_DESC1 description{};
		const auto descriptionResult = CheckHResult(swapChain->GetDesc1(&description), "스왑 체인 설정 조회");
		if (!descriptionResult)
		{
			Engine::GetInstance().ReportError(descriptionResult.error());
			return;
		}
		for (auto& buffer : buffers)
		{
			buffer.Reset();
		}
		const auto result = CheckHResult(
			swapChain->ResizeBuffers(static_cast<UINT>(buffers.size()), targetWidth, targetHeight,
				DXGI_FORMAT_R8G8B8A8_UNORM, description.Flags), "스왑 체인 크기 변경");
		if (!result)
		{
			Engine::GetInstance().ReportError(result.error());
			return;
		}
		const auto targets = CreateRenderTargets();
		if (!targets)
		{
			Engine::GetInstance().ReportError(targets.error());
			return;
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
