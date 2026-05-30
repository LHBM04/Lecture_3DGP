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

#include "Matrix4x4.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "Singleton.h"

class Camera;
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

	bool BeginFrame();
	void EndFrame();

	void Present();

	void SetCamera(Camera* camera_);
	void SetLights(std::span<Light*> lights_);

	// Low-level API
	void SetPipelineState(ID3D12PipelineState* pipelineState_);
	void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature_);
	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vbv_);
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& ibv_);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology_);
	void SetGraphicsRootConstantBufferView(UINT rootParameterIndex_, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_);
	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex_, D3D12_GPU_DESCRIPTOR_HANDLE baseDescriptor_);
	
	void DrawInstanced(UINT vertexCountPerInstance_, UINT instanceCount_, UINT startVertexLocation_, UINT startInstanceLocation_);
	void DrawIndexedInstanced(UINT indexCountPerInstance_, UINT instanceCount_, UINT startIndexLocation_, INT baseVertexLocation_, UINT startInstanceLocation_);

public:
	struct alignas(256) CameraConstants final
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 projectionMatrix;
	};

	struct alignas(256) LightConstants final
	{
		Vector4D ambientColor;
		Vector4D lights[8]; // color(rgb), intensity(a)
		Vector4D lightDirs[8]; // direction(xyz), active(a)
		Vector3D cameraPosition;
		uint32_t activeLightCount;
	};

	struct alignas(256) ObjectConstants final
	{
		Matrix4x4 worldMatrix;
	};

	struct alignas(256) MaterialConstants final
	{
		Vector4D color;
	};

	void SetObjectConstants(const ObjectConstants& data_);
	void SetMaterialConstants(const MaterialConstants& data_);

	template<class T>
	D3D12_GPU_VIRTUAL_ADDRESS UploadConstantsData(const T& data_);

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

	void WaitForGpu();
	void MoveToNextFrame();

public:
	static constexpr UINT MaxLights{ 8 };
	static constexpr UINT MaxInstances{ 1024 };

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;
	
	[[nodiscard]] ID3D12RootSignature* GetDefaultRootSignature() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetDefaultPipelineState() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetLightingPipelineState() const noexcept;

private:
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

	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;

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
	static constexpr UINT MaxConstantBufferSize{ 1024 * 1024 * 8 };
};

template<class T>
inline D3D12_GPU_VIRTUAL_ADDRESS RenderSystem::UploadConstantsData(const T& data_)
{
	const UINT size{ (sizeof(T) + 255) & ~255 };

	assert(mappedConstantData != nullptr);
	assert(constantBufferOffset + size <= (frameIndex + 1) * MaxConstantBufferSize);

	std::memcpy(mappedConstantData + constantBufferOffset, &data_, sizeof(T));

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ constantBuffer->GetGPUVirtualAddress() + constantBufferOffset };
	constantBufferOffset += size;

	return gpuAddress;
}
