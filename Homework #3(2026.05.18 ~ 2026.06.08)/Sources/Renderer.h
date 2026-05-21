#pragma once

#include "FrameResource.h"
#include "RenderTarget.h"

class Renderer final
{
public:
	static constexpr UINT FrameCount{ 2 };

	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	~Renderer();

	bool Initialize(HWND windowHandle, UINT width, UINT height);
	void Render();
	void Shutdown() noexcept;

	void WaitForGPU();

private:
	void EnableDebugLayer();
	bool CreateDevice();
	bool CreateCommandObjects();
	bool CreateSwapChain(HWND windowHandle, UINT width, UINT height);
	bool CreateRenderTargets();
	bool CreateFence();
	void MoveToNextFrame();
	void ThrowIfFailed(HRESULT result, std::string_view message) const;

	Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;

	std::array<RenderTarget, FrameCount> renderTargets;
	std::array<FrameResource, FrameCount> frameResources;

	HANDLE fenceEvent{ nullptr };
	UINT frameIndex{ 0 };
	UINT rtvDescriptorSize{ 0 };
	bool initialized{ false };
};
