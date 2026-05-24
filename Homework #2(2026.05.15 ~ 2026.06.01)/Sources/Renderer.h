#pragma once

#include "Singleton.h"
#include "CameraConstants.h"
#include "DrawCall.h"
#include "RendererOptions.h"

#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <vector>
#include <span>

class Shader;
class Material;
class Mesh;
class Camera;
class GameObject;

class Renderer final : public Singleton<Renderer>
{
public:
	bool Initialize(const RendererOptions& options_);
	void Release();

	void Clear();
	
	void BeginRender();
	void EndRender();

	void SetCamera(const Camera* camera_);
	void SetObject(const GameObject* gameObject_);

	void BindPipeline(const Shader& shader_);
	void BindMaterial(const Material& material_);
	void BindMesh(const Mesh& mesh_);

	void Render();
	void Flush();
	void WaitIdle();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;

private:
	struct BackBuffer final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		D3D12_RESOURCE_STATES state;
	};

	struct FrameResource final
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
		UINT8* mappedUploadBuffer;

		std::size_t capacity;
		std::size_t offset;

		UINT64 fenceValue;
	};

	struct InstanceData final
	{
		Matrix4x4 worldTransform;
	};

	struct Batch final
	{
		DrawCall baseCall;
		std::vector<InstanceData> instances;
	};

	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateBackBuffers();
	void CreateDepthStencilBuffer();
	void CreateFrameResources();
	void CreateCommandList();
	void CreateFence();

	void TransitionBackBuffer(D3D12_RESOURCE_STATES state_);

	void BindCameraConstants();
	void BindObjectConstants();

	void BuildVisibleDrawCalls();
	void SortDrawCalls();
	
	void BuildBatches();
	void ExecuteBatches();
	void ExecuteBatch(const Batch& batch_);

	[[nodiscard]] bool IsValidDrawCall(const DrawCall& drawCall_) const noexcept;
	[[nodiscard]] uint64_t BuildSortKey(const DrawCall& drawCall_) const noexcept;
	[[nodiscard]] bool CanBatchTogether(const DrawCall& a_, const DrawCall& b_) const noexcept;

	[[nodiscard]] void* AllocateUploadMemory(std::size_t sizeInBytes_, std::size_t alignment_, D3D12_GPU_VIRTUAL_ADDRESS& outGpuAddress_);
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UploadConstantData(const void* data_, std::size_t sizeInBytes_);
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW UploadInstanceData(std::span<const InstanceData> instances_);

	UINT64 SignalFence();
	void WaitForFrame(FrameResource& frame_);

	static constexpr std::size_t FrameCount{ 2 };
	static constexpr std::size_t UploadBufferSize{ 4u * 1024u * 1024u };

	RendererOptions options;

#if defined(_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	std::array<BackBuffer, FrameCount> backBuffers;
	std::size_t currentBackBufferIndex;

	std::array<FrameResource, FrameCount> frameResources;
	std::size_t currentFrameResourceIndex;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	UINT rtvDescriptorOffset;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	UINT dsvDescriptorOffset;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 nextFenceValue;
	HANDLE fenceEvent;

	CameraConstants currentCameraConstants{};
	DrawCall currentDrawCall{};

	std::vector<DrawCall> drawCalls;
	std::vector<DrawCall> visibleDrawCalls;
	std::vector<Batch> batches;
};
