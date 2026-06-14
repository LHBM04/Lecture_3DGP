#pragma once

#include <array>
#include <cstddef>
#include <expected>
#include <memory>
#include <unordered_map>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Service.h"

class RenderService : public Service
{
public:
	struct Options final
	{
		HWND hWnd;
		int x;
		int y;
		int width;
		int height;
		std::size_t bufferCount;
		bool isEnableMSAAx4;
	};

	~RenderService() noexcept override = default;

	bool Initialize(const Options& options_);
	void Terminate() noexcept;

	[[nodiscard]] ID3D12Device* GetD3DDevice() const noexcept;

protected:
	void OnAdd() override;
	void OnRemove() override;

private:
	HRESULT CreateDevice();

	Options options;

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> graphicsRootSignature;
	
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	std::size_t currentBufferIndex;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	std::size_t rtvDescriptorIncrementSize;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	std::size_t	dsvDescriptorIncrementSize;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	std::size_t srvDescriptorIncrementSize;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::vector<UINT64> fenceValues;
	HANDLE fenceEvent;
};
