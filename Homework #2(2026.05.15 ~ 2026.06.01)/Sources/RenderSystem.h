#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "Singleton.h"
#include "Matrix4x4.h"
#include "Vector4D.h"

class Camera;
class GameObject;
class Light;
class Mesh;
class Material;

class RenderSystem final : public Singleton<RenderSystem>
{
public:
	RenderSystem() = default;
	~RenderSystem() override = default;

	std::expected<void, std::wstring> Initialize(HWND window_);
	void Release();

	void BeginFrame() noexcept;
	void EndFrame() noexcept;

	void Present() noexcept;

	void SetCamera(Camera* camera_) noexcept;
	void SetLights(std::span<Light*> lights_) noexcept;

	// Low-level API
	void SetPipelineState(ID3D12PipelineState* pipelineState_) noexcept;
	void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature_) noexcept;
	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vbv_) noexcept;
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv_) noexcept;
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology_) noexcept;
	void SetGraphicsRootConstantBufferView(UINT rootParameterIndex_, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_) noexcept;
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex_, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor_) noexcept;
	
	void DrawInstanced(UINT vertexCountPerInstance_, UINT instanceCount_, UINT startVertexLocation_, UINT startInstanceLocation_) noexcept;
	void DrawIndexedInstanced(UINT indexCountPerInstance_, UINT instanceCount_, UINT startIndexLocation_, INT baseVertexLocation_, UINT startInstanceLocation_) noexcept;

	template<class T>
	D3D12_GPU_VIRTUAL_ADDRESS UploadConstantsData(const T& data_) noexcept;

private:
	std::expected<void, std::wstring> CreateDevice();
	std::expected<void, std::wstring> CreateCommandQueue();
	std::expected<void, std::wstring> CreateSwapChain(HWND window_);
	std::expected<void, std::wstring> CreateDescriptorHeaps();
	std::expected<void, std::wstring> CreateCommandList();
	std::expected<void, std::wstring> CreateSyncObjects();
	std::expected<void, std::wstring> CreateConstantBuffer();
	std::expected<void, std::wstring> CreateRootSignature();
	std::expected<void, std::wstring> CreatePipelineState();
	std::expected<void, std::wstring> CreateGBuffers();

	void WaitForGpu() noexcept;
	void MoveToNextFrame() noexcept;

public:
	static constexpr UINT MaxLights{ 8 };
	static constexpr UINT MaxInstances{ 1024 };

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;
	
	[[nodiscard]] ID3D12RootSignature* GetDefaultRootSignature() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetDefaultPipelineState() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetLightingPipelineState() const noexcept;

private:
	struct CameraConstants final
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 projectionMatrix;
	};

	struct LightData final
	{
		Vector4D color;
		Vector4D direction;
	};

	struct LightConstants final
	{
		Vector4D ambientColor;
		Vector4D cameraPosition;
		LightData lights[MaxLights];
		uint32_t activeLightCount;
		float padding[3];
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

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	UINT srvDescriptorSize{ 0 };

	struct GBuffer final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	};

	std::array<GBuffer, 2> gBuffers; // 0: Albedo, 1: Normal
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> gBufferRtvHeap;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> lightingPipelineState;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::array<UINT64, FrameCount> fenceValues{ 0 };
	HANDLE fenceEvent{ nullptr };

	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, 0, 0 };

	Camera* camera{ nullptr };
	UINT width{ 0 };
	UINT height{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
	uint8_t* mappedConstantData{ nullptr };
	UINT constantBufferOffset{ 0 };
	static constexpr UINT MaxConstantBufferSize{ 1024 * 128 };
};

template<class T>
inline D3D12_GPU_VIRTUAL_ADDRESS RenderSystem::UploadConstantsData(const T& data_) noexcept
{
	const UINT size{ (sizeof(T) + 255) & ~255 };

	assert(mappedConstantData != nullptr);
	assert(constantBufferOffset + size <= (frameIndex + 1) * MaxConstantBufferSize);

	std::memcpy(mappedConstantData + constantBufferOffset, &data_, sizeof(T));

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ constantBuffer->GetGPUVirtualAddress() + constantBufferOffset };
	constantBufferOffset += size;

	return gpuAddress;
}

inline ID3D12Device* RenderSystem::GetDevice() const noexcept
{
	return device.Get();
}

inline const D3D12_VIEWPORT& RenderSystem::GetViewport() const noexcept
{
	return viewport;
}

inline ID3D12RootSignature* RenderSystem::GetDefaultRootSignature() const noexcept
{
	return rootSignature.Get();
}

inline ID3D12PipelineState* RenderSystem::GetDefaultPipelineState() const noexcept
{
	return pipelineState.Get();
}

inline ID3D12PipelineState* RenderSystem::GetLightingPipelineState() const noexcept
{
	return lightingPipelineState.Get();
}
