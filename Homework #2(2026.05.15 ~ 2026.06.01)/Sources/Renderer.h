#pragma once

#include "Singleton.h"
#include "CameraConstants.h"
#include "CameraClearMode.h"
#include "ColorRGB.h"
#include "ColorRGBA.h"
#include "Matrix4x4.h"
#include "System.h"
#include "Vector3D.h"

#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <vector>
#include <span>
#include <string>

class Shader;
class Material;
class Mesh;
class Camera;
class GameObject;
class Light;

struct RendererOptions final
{
	HWND window;
	int x;
	int y;
	int width;
	int height;
	bool msaa4xEnable;
	bool enableTripleBuffering;
	bool vSync;
	bool fullscreen;
};

class Renderer final : public System<RendererOptions>
{
public:
	~Renderer() noexcept override;

	bool Initialize(const RendererOptions& options_) override;
	void Release() override;

	void Clear();
	void ResetViewport();
	
	void BeginRender();
	void EndRender();

	void SetCamera(const Camera* camera_);
	void SetLight(const Light* light_);

	void UseProgram(Shader* shader_);
	void BindVertexBuffer(
		const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView_,
		UINT vertexCount_,
		uint64_t meshId_,
		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		UINT firstVertex_ = 0);
	void BindElementBuffer(
		const D3D12_INDEX_BUFFER_VIEW& indexBufferView_,
		UINT indexCount_,
		UINT firstIndex_ = 0,
		INT baseVertexLocation_ = 0);
	void BindMaterial(const Material* material_, const ColorRGBA* overrideColor_ = nullptr);
	void SetModelMatrix(const Matrix4x4& modelMatrix_);
	void DrawArrays();
	void DrawElements();

	void Flush();
	void WaitForFrames();
	void SetFullscreen(bool fullscreen_);
	void ToggleFullscreen();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] bool IsFullscreen() const noexcept;
	[[nodiscard]] int GetWidth() const noexcept;
	[[nodiscard]] int GetHeight() const noexcept;

private:
	struct DrawCall final
	{
		ID3D12PipelineState* pipelineState{ nullptr };
		ID3D12RootSignature* graphicsRootSignature{ nullptr };

		D3D12_PRIMITIVE_TOPOLOGY primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
		D3D12_INDEX_BUFFER_VIEW indexBufferView{};

		bool hasIndexBuffer{ false };
		bool indexed{ false };

		D3D12_GPU_DESCRIPTOR_HANDLE materialDescriptorTable{};
		ColorRGBA materialColor{ ColorRGBA::GetWhite() };

		UINT vertexCount{ 0 };
		UINT startVertexLocation{ 0 };

		UINT indexCount{ 0 };
		UINT startIndexLocation{ 0 };
		INT baseVertexLocation{ 0 };

		UINT instanceCount{ 1 };
		UINT startInstanceLocation{ 0 };

		uint64_t pipelineId{ 0 };
		uint64_t materialId{ 0 };
		uint64_t meshId{ 0 };

		Matrix4x4 worldTransform{ Matrix4x4::GetIdentity() };

		uint64_t sortKey{ 0 };
	};

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
	void CreateHardwareDevice();
	void CreateWarpDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateDescriptorHeaps();
	void CreateBackBuffers();
	void CreateDepthStencilBuffer();
	void CreateFrameResources();
	void CreateCommandList();
	void CreateFence();

	void ReleaseBackBuffers();
	void ReleaseDepthStencilBuffer();
	void ApplyFullscreenState();
	void TransitionBackBuffer(D3D12_RESOURCE_STATES state_);

	void BindCameraConstants();
	void BindLightConstants();

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
	bool isFullscreen;
	int windowedWidth;
	int windowedHeight;

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
	struct LightConstants final
	{
		Vector3D direction;
		float intensity;
		ColorRGB color;
		float padding;
	};

	Vector3D currentLightDirection{};
	ColorRGB currentLightColor{};
	float currentLightIntensity{};
	CameraClearMode currentCameraClearMode{ CameraClearMode::SolidColor };
	ColorRGBA currentCameraClearColor{ ColorRGBA::GetBlue() };

	std::vector<DrawCall> drawCalls;
	std::vector<DrawCall> visibleDrawCalls;
	std::vector<Batch> batches;
	DrawCall drawState{};
};
