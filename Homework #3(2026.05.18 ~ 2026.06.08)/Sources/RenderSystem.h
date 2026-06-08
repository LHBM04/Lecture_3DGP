#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl.h>

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Matrix4x4.h"
#include "Singleton.h"

struct CameraConstants;
struct LightConstants;
struct ObjectConstants;
struct MaterialConstants;
class ColorRGBA;

class Material;
class Mesh;

class RenderSystem : public Singleton<RenderSystem>
{
public:
	RenderSystem() noexcept = default;
	~RenderSystem() noexcept = default;

	bool Initialize(HWND window_);
	void Release();

	void PreRender();
	void Render();
	void PostRender();

	void Clear(const ColorRGBA& clearColor_);
	void Present();

	void SetCameraConstants(const CameraConstants& constants_);
	void SetLightConstants(const LightConstants& constants_);
	void SetObjectConstants(const ObjectConstants& constants_);
	void SetMaterialConstants(const MaterialConstants& constants_);

	void SubmitRenderRequest(Mesh* mesh_, Material* material_, const Matrix4x4& worldMatrix_);
	void DrawMesh(Mesh* mesh_, Material* material_, const Matrix4x4& worldMatrix_);
	void DrawMeshInstanced(Mesh* mesh_, Material* material_, std::span<const Matrix4x4> worldMatrices_);

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const noexcept;
	[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;

private:
	enum class RootParameter : std::uint8_t
	{
		Camera = 0,
		Object = 1,
		Light = 2,
		Material = 3
	};

	bool CreateDevice();
	bool CreateGraphicsRootSignature();
	bool CreateCommandObjects();
	bool CreateSwapChain(HWND window_);
	bool CreateRenderTargetViews();
	bool CreateDepthStencilView();
	bool CreateFence();
	bool CreateConstantBuffers();

	void WaitForGPU();
	void MoveToNextFrame();
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UploadConstantData(const void* data_, UINT sizeInBytes_);
	[[nodiscard]] D3D12_VERTEX_BUFFER_VIEW UploadInstanceWorldMatrices(std::span<const Matrix4x4> worldMatrices_);
	[[nodiscard]] static UINT AlignConstantBufferSize(UINT sizeInBytes_) noexcept;
	[[nodiscard]] static UINT AlignBufferSize(UINT sizeInBytes_, UINT alignment_) noexcept;

	static constexpr std::size_t BackBufferCount{ 2 };
	static constexpr std::size_t MaxConstantBufferSize{ 1024 * 1024 * 8 }; // 8MB
	static constexpr std::size_t MaxInstanceBufferSize{ 1024 * 1024 * 16 }; // 16MB

	struct FrameConstantBuffer final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::byte* mappedData{ nullptr };
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
		UINT currentOffset{ 0 };
	};

	struct FrameInstanceBuffer final
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		std::byte* mappedData{ nullptr };
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ 0 };
		UINT currentOffset{ 0 };
	};

	struct RenderRequest final
	{
		Mesh* mesh{ nullptr };
		Material* material{ nullptr };
		Matrix4x4 worldMatrix;
	};

	Microsoft::WRL::ComPtr<IDXGIFactory7> factory;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> graphicsRootSignature;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, BackBufferCount> commandAllocators;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	UINT frameIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	uint32_t rtvDescriptorSize{ 0 };
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	uint32_t dsvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	uint32_t srvDescriptorSize{ 0 };

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, BackBufferCount> renderTargets;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	std::array<std::uint64_t, BackBufferCount> fenceValues;
	HANDLE fenceEvent{ nullptr };

	std::array<FrameConstantBuffer, BackBufferCount> constantBuffers;
	std::array<FrameInstanceBuffer, BackBufferCount> instanceBuffers;
	std::vector<RenderRequest> renderRequests;
	std::vector<Matrix4x4> batchedWorldMatrices;
};
