#pragma once
#pragma once

#include <array>
#include <cstddef>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

#include "Singleton.h"

class Camera;
class GameObject;

class RenderSystem final : public Singleton<RenderSystem>
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

	void SetCamera(Camera* camera_);
	void SetObject(GameObject* object_);

private:
	void WaitForGpu();
	void MoveToNextFrame();

	void CreateRootSignature();
	void CreatePipelineState();
	void CreateMesh();

private:
	struct Vertex final
	{
		Vector3D position;
		ColorRGBA color;
	};

	struct CameraConstants final
	{
	};

	struct LightConstants final
	{
	};

	struct ObjectConstants final
	{
	};

	static constexpr std::size_t FrameCount{ 2 };

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, FrameCount> backBuffers;
	UINT frameIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, FrameCount> commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	UINT rtvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	UINT dsvDescriptorSize{ 0 };
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::array<UINT64, FrameCount> fenceValues{ 0 };
	HANDLE fenceEvent{ nullptr };

	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, 0, 0 };
};
