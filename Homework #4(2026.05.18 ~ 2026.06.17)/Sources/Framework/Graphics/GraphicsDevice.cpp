#include "Precompiled.h"
#include "GraphicsDevice.h"

#include "GraphicsError.h"

namespace TUK::Framework
{
	GraphicsDevice::GraphicsDevice() noexcept
		: factory()
		, device()
	{
	}

	std::expected<void, std::string> GraphicsDevice::Initialize()
	{
		if (factory || device)
		{
			return std::unexpected(std::string("GraphicsDevice가 이미 초기화되어 있습니다."));
		}
		// 실패한 초기화에서 일부 자원을 멤버에 남기지 않는다.
		Microsoft::WRL::ComPtr<IDXGIFactory6> createdFactory;
		Microsoft::WRL::ComPtr<ID3D12Device> createdDevice;
		UINT factoryFlags = 0;
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()))))
		{
			debug->EnableDebugLayer();
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		const auto factoryResult = CheckHResult(CreateDXGIFactory2(factoryFlags,
			IID_PPV_ARGS(createdFactory.GetAddressOf())), "DXGI 팩토리 생성");
		if (!factoryResult)
		{
			return factoryResult;
		}
		for (UINT index = 0; ; ++index)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			const HRESULT result = createdFactory->EnumAdapterByGpuPreference(index,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(adapter.GetAddressOf()));
			if (result == DXGI_ERROR_NOT_FOUND)
			{
				break;
			}
			const auto adapterResult = CheckHResult(result, "어댑터 조회");
			if (!adapterResult)
			{
				return adapterResult;
			}
			DXGI_ADAPTER_DESC1 description{};
			const auto descriptionResult = CheckHResult(adapter->GetDesc1(&description), "어댑터 정보 조회");
			if (!descriptionResult)
			{
				return descriptionResult;
			}
			if (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
			if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(createdDevice.ReleaseAndGetAddressOf()))))
			{
				break;
			}
		}
		if (!createdDevice)
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> warp;
			const auto warpResult = CheckHResult(createdFactory->EnumWarpAdapter(
				IID_PPV_ARGS(warp.GetAddressOf())), "WARP 어댑터 조회");
			if (!warpResult)
			{
				return warpResult;
			}
			const auto deviceResult = CheckHResult(D3D12CreateDevice(warp.Get(), D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(createdDevice.ReleaseAndGetAddressOf())), "D3D12 디바이스 생성");
			if (!deviceResult)
			{
				return deviceResult;
			}
		}
		factory = std::move(createdFactory);
		device = std::move(createdDevice);
		return {};
	}

	void GraphicsDevice::Release() noexcept
	{
		device.Reset();
		factory.Reset();
	}

	ID3D12Device& GraphicsDevice::GetNativeDevice() const noexcept
	{
		assert(device);
		return *device.Get();
	}

	std::expected<SwapChain, std::string> GraphicsDevice::CreateSwapChain(
		Window& window, ID3D12CommandQueue& queue, UINT width, UINT height) const
	{
		assert(device && factory);
		SwapChain swapChain(window);
		return swapChain.Initialize(*device.Get(), *factory.Get(), queue, width, height).transform(
			[&swapChain] { return std::move(swapChain); });
	}

	std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> GraphicsDevice::CreateResource(
		const D3D12_RESOURCE_DESC& description, D3D12_HEAP_TYPE heapType,
		D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue) const
	{
		assert(device);
		D3D12_HEAP_PROPERTIES heap{};
		heap.Type = heapType;
		heap.CreationNodeMask = 1;
		heap.VisibleNodeMask = 1;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		const auto result = CheckHResult(device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE,
			&description, initialState, clearValue, IID_PPV_ARGS(resource.GetAddressOf())), "리소스 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return resource;
	}

	std::expected<Buffer, std::string> GraphicsDevice::CreateBuffer(
		UINT64 size, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags) const
	{
		D3D12_RESOURCE_DESC description{};
		description.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		description.Width = size;
		description.Height = 1;
		description.DepthOrArraySize = 1;
		description.MipLevels = 1;
		description.SampleDesc.Count = 1;
		description.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		description.Flags = flags;
		return CreateResource(description, heapType, initialState, nullptr).transform(
			[size, heapType](auto resource)
			{
				return Buffer(std::move(resource), size, heapType);
			});
	}

	std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> GraphicsDevice::CreateTexture(
		const D3D12_RESOURCE_DESC& description, D3D12_RESOURCE_STATES initialState,
		const D3D12_CLEAR_VALUE* clearValue) const
	{
		if (description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE1D
			&& description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D
			&& description.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE3D)
		{
			return std::unexpected(std::string("텍스처 리소스 설명이 필요합니다."));
		}
		return CreateResource(description, D3D12_HEAP_TYPE_DEFAULT, initialState, clearValue);
	}

	std::expected<Microsoft::WRL::ComPtr<ID3D12CommandQueue>, std::string> GraphicsDevice::CreateCommandQueue(
		const D3D12_COMMAND_QUEUE_DESC& description) const
	{
		assert(device);
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> queue;
		const auto result = CheckHResult(device->CreateCommandQueue(&description,
			IID_PPV_ARGS(queue.GetAddressOf())), "명령 큐 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return queue;
	}

	std::expected<Fence, std::string> GraphicsDevice::CreateFence(
		UINT64 initialValue, D3D12_FENCE_FLAGS flags) const
	{
		assert(device);
		Fence fence;
		return fence.Initialize(*device.Get(), initialValue, flags).transform(
			[&fence] { return std::move(fence); });
	}

	std::expected<Microsoft::WRL::ComPtr<ID3D12RootSignature>, std::string> GraphicsDevice::CreateRootSignature(
		std::span<const std::byte> serializedSignature) const
	{
		assert(device);
		Microsoft::WRL::ComPtr<ID3D12RootSignature> signature;
		const auto result = CheckHResult(device->CreateRootSignature(0, serializedSignature.data(),
			serializedSignature.size(), IID_PPV_ARGS(signature.GetAddressOf())), "루트 시그니처 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return signature;
	}

	std::expected<GraphicsPipeline, std::string> GraphicsDevice::CreateGraphicsPipeline(
		const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const
	{
		assert(device && description.pRootSignature);
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state;
		const auto result = CheckHResult(device->CreateGraphicsPipelineState(&description,
			IID_PPV_ARGS(state.GetAddressOf())), "그래픽 파이프라인 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return GraphicsPipeline(std::move(state), *description.pRootSignature);
	}

	std::expected<ComputePipeline, std::string> GraphicsDevice::CreateComputePipeline(
		const D3D12_COMPUTE_PIPELINE_STATE_DESC& description) const
	{
		assert(device && description.pRootSignature);
		Microsoft::WRL::ComPtr<ID3D12PipelineState> state;
		const auto result = CheckHResult(device->CreateComputePipelineState(&description,
			IID_PPV_ARGS(state.GetAddressOf())), "컴퓨트 파이프라인 생성");
		if (!result)
		{
			return std::unexpected(result.error());
		}
		return ComputePipeline(std::move(state), *description.pRootSignature);
	}
}
