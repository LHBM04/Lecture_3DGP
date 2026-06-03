#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

struct RenderTarget final
{
	int width{ 0 };
	int height{ 0 };

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[2];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValues[2]{ 0 };
	HANDLE fenceEvent{ nullptr };

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};

	UINT frameIndex{ 0 };
};
