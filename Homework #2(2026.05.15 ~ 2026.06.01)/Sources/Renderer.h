#pragma once

#include "Singleton.h"
#include "CameraClearMode.h"
#include "ColorRGB.h"
#include "ColorRGBA.h"
#include "Matrix4x4.h"
#include "RenderContext.h"
#include "Vector3D.h"

#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <memory>
#include <vector>
#include <span>
#include <string>

class Shader;
class Material;
class Mesh;
class Camera;
class GameObject;
class Light;
class RenderSystem;

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

class Renderer final
{
	friend class RenderSystem;
	friend class RenderContext;

public:
	~Renderer() noexcept;
	
	void BeginRender();
	void EndRender();

	void Flush();
	void FlushGameObjects();
	void FlushUIObjects();
	void Resize(int width_, int height_);
	void WaitForFrames();
	void SetFullscreen(bool fullscreen_);
	void ToggleFullscreen();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] bool IsFullscreen() const noexcept;
	[[nodiscard]] int GetWidth() const noexcept;
	[[nodiscard]] int GetHeight() const noexcept;
	[[nodiscard]] RenderContext& GetContext() noexcept;

private:
	struct GameObjectCommand
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

		UINT cameraSlot{ UINT_MAX };
		UINT objectSlot{ UINT_MAX };
		UINT materialSlot{ UINT_MAX };
		UINT lightSlot{ UINT_MAX };
	};

	struct UIObjectCommand : GameObjectCommand
	{
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

	struct GameObjectBatch final
	{
		GameObjectCommand baseCall;
		std::vector<Matrix4x4> instances;
	};

	struct UIObjectBatch final
	{
		UIObjectCommand baseCall;
		std::vector<Matrix4x4> instances;
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

	void BindCameraConstants(UINT slot_);
	void BindObjectConstants(UINT slot_, const Matrix4x4& world_);
	void BindLightConstants(UINT slot_);
	void BindMaterialConstants(UINT slot_, const ColorRGBA& materialColor_);

	void BuildVisibleGameObjectCommands();
	void SortGameObjectCommands();
	void SortUIObjectCommands();
	
	void BuildGameObjectBatches();
	void BuildUIObjectBatches();
	void ExecuteGameObjectBatches();
	void ExecuteUIObjectBatches();
	void ExecuteBatch(const GameObjectBatch& batch_);
	void ExecuteBatch(const UIObjectBatch& batch_);

	[[nodiscard]] bool IsValidCommand(const GameObjectCommand& command_) const noexcept;
	[[nodiscard]] uint64_t BuildSortKey(const GameObjectCommand& command_) const noexcept;
	[[nodiscard]] bool CanBatchTogether(const GameObjectCommand& a_, const GameObjectCommand& b_) const noexcept;

	[[nodiscard]] void* AllocateUploadMemory(std::size_t sizeInBytes_, std::size_t alignment_, D3D12_GPU_VIRTUAL_ADDRESS& outGpuAddress_);
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UploadConstantData(const void* data_, std::size_t sizeInBytes_);
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW UploadInstanceData(std::span<const Matrix4x4> instances_);

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

	struct CameraConstants final
	{
		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 viewProjection;
	};
	CameraConstants currentCameraConstants{};
	struct ObjectConstants final
	{
		Matrix4x4 world;
		Matrix4x4 inverseWorld;
	};
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

	std::vector<GameObjectCommand> gameObjectCommands;
	std::vector<GameObjectCommand> visibleGameObjectCommands;
	std::vector<UIObjectCommand> uiObjectCommands;
	std::vector<GameObjectBatch> gameObjectBatches;
	std::vector<UIObjectBatch> uiObjectBatches;
	GameObjectCommand drawState{};
	std::unique_ptr<RenderContext> context;

	bool Initialize(const RendererOptions& options_);
	void Release();
};
