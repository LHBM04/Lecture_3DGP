#pragma once

#include <array>

#include <wrl.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Singleton.h"

class RenderSystem : public Singleton<RenderSystem>
{
public:
	struct alignas(256) CameraConstants final
	{
		// Matrix4x4 viewMatrix;
		// Matrix4x4 projectionMatrix;
	};

	struct alignas(256) LightConstants final
	{
		// Vector4D ambientColor;
		// Vector4D lights[8];
		// Vector4D lightDirs[8];
		// Vector3D cameraPosition;
		// uint32_t activeLightCount;
	};

	struct alignas(256) ObjectConstants final
	{
		// Matrix4x4 worldMatrix;
	};

	struct alignas(256) MaterialConstants final
	{
		// ColorRGBA color;
	};

	RenderSystem() = default;
	~RenderSystem() override = default;

	bool Initialize(HWND window_);
	void Release();

	void BeginFrame();
	void EndFrame();

	void Clear();
	void Present();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const noexcept;

private:
	static constexpr UINT BackBufferCount{ 2 };

	HRESULT CreateDevice();
	HRESULT CreateCommandObjects();
	HRESULT CreateSwapChain(HWND hWnd_);
	HRESULT CreateDescriptorHeaps();
	HRESULT CreateSyncObjects();

	HRESULT CreateRenderTargetViews();
	HRESULT CreateDepthStencilView();

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> device;

	bool isEnableMsaa4x{ false };
	UINT msaa4xQualityLevels{ 0 };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BackBufferCount> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::array<UINT64, BackBufferCount> fenceValues{ 0 };
	HANDLE fenceEvent{ nullptr };

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

	UINT frameIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvDescriptorSize{ 0 };
	UINT rtvHeapOffset{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT dsvDescriptorSize{ 0 };
	UINT dsvHeapOffset{ 0 };
};
