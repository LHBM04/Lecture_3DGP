#pragma once

#include <array>

#include <wrl.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Singleton.h"

class Camera;
class Light;
struct CameraConstants;
struct LightConstants;
struct GameObjectConstants;
class Material;
struct MaterialConstants;

class RenderSystem : public Singleton<RenderSystem>
{
public:

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
	void SetCameraConstants(const CameraConstants& data_);
	void SetLightConstants(const LightConstants& data_);
	void SetObjectConstants(const GameObjectConstants& data_);
	void SetMaterialConstants(const MaterialConstants& data_);

private:
	static constexpr UINT BackBufferCount{ 2 };
	static constexpr UINT FrameConstantBufferSize{ 64 * 1024 };

	struct FrameConstantBuffer final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::byte* mappedData{ nullptr };
		D3D12_GPU_VIRTUAL_ADDRESS gpuBaseAddress{ 0 };
		UINT currentOffset{ 0 };
	};

	void WaitForGpu();
	void MoveToNextFrame();
	[[nodiscard]] D3D12_RESOURCE_BARRIER CreateTransitionBarrier(
		ID3D12Resource* resource_,
		D3D12_RESOURCE_STATES before_,
		D3D12_RESOURCE_STATES after_) const noexcept;

	HRESULT CreateDevice();
	HRESULT CreateCommandObjects();
	HRESULT CreateSwapChain(HWND hWnd_);
	HRESULT CreateDescriptorHeaps();
	HRESULT CreateSyncObjects();
	HRESULT CreateConstantBuffers();

	HRESULT CreateRenderTargetViews();
	HRESULT CreateDepthStencilView();

	D3D12_GPU_VIRTUAL_ADDRESS UploadConstantData(const void* data_, UINT sizeInBytes_);
	static UINT AlignConstantBufferSize(UINT sizeInBytes_) noexcept;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> device;

	bool isEnableMsaa4x{ false };
	UINT msaa4xQualityLevels{ 0 };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, BackBufferCount> commandAllocators;
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
	UINT clientWidth{ 0 };
	UINT clientHeight{ 0 };

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT dsvDescriptorSize{ 0 };

	std::array<FrameConstantBuffer, BackBufferCount> frameConstantBuffers;
};
