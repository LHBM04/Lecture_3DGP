#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <expected>
#include <span>
#include <string>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <windows.h>
#include <wrl.h>

#include "ColorRGBA.h"
#include "Matrix4x4.h"
#include "Singleton.h"
#include "Vector3D.h"
#include "Vector4D.h"

class Camera;
class Light;
class Mesh;
class Material;

class RenderSystem final : public Singleton<RenderSystem>
{
	friend class Singleton<RenderSystem>;

public:
	struct alignas(256) CameraConstants final
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 projectionMatrix;
	};

	struct alignas(256) ObjectConstants final
	{
		Matrix4x4 worldMatrix;
	};

	struct alignas(256) MaterialConstants final
	{
		ColorRGBA color;
	};

	struct alignas(256) LightConstants final
	{
		Vector4D ambientColor;
		Vector4D lights[8];
		Vector4D lightDirs[8];
		Vector3D cameraPosition;
		uint32_t activeLightCount;
	};

public:
	RenderSystem() = default;
	~RenderSystem() override = default;

	std::expected<void, std::wstring> Initialize(HWND hWnd_);
	void Release();

	bool BeginFrame();
	void EndFrame();
	void Present();

	void SetCamera(Camera* camera_);
	void SetLights(std::span<Light* const> lights_);

	void SetPipelineState(ID3D12PipelineState* pipelineState_);
	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView_);
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView_);
	
	void DrawIndexedInstanced(UINT indexCountPerInstance_, UINT instanceCount_, UINT startIndexLocation_, INT baseVertexLocation_, UINT startInstanceLocation_);
	void DrawInstanced(UINT vertexCountPerInstance_, UINT instanceCount_, UINT startVertexLocation_, UINT startInstanceLocation_);

	void ExecuteLightingPass();
	void SetRenderTargetToBackBuffer();

	[[nodiscard]] ID3D12Device* GetDevice() const noexcept;
	[[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const noexcept;
	[[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept;

	[[nodiscard]] ID3D12RootSignature* GetDefaultRootSignature() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetDefaultPipelineState() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetUIPipelineState() const noexcept;
	[[nodiscard]] ID3D12PipelineState* GetLightingPipelineState() const noexcept;

	void SetObjectConstants(const ObjectConstants& data_);
	void SetMaterialConstants(const MaterialConstants& data_);
	void DrawMeshInstanced(Mesh* mesh_, Material* material_, std::span<const Matrix4x4> worldMatrices_);

private:
	std::expected<void, std::wstring> CreateDevice();
	std::expected<void, std::wstring> CreateCommandObjects();
	std::expected<void, std::wstring> CreateSwapChain(HWND hWnd_);
	std::expected<void, std::wstring> CreateDescriptorHeaps();
	std::expected<void, std::wstring> CreateGBuffers();
	std::expected<void, std::wstring> CreateRootSignature();
	std::expected<void, std::wstring> CreatePipelineStates();
	std::expected<void, std::wstring> CreateConstantBuffer();
	std::expected<void, std::wstring> CreateSyncObjects();

	template <class T>
	[[nodiscard]] D3D12_GPU_VIRTUAL_ADDRESS UploadConstantsData(const T& data_);

	void MoveToNextFrame();
	void WaitForGpu();

private:
	static constexpr uint32_t SwapChainBufferCount{ 2 };
	static constexpr uint32_t MaxConstantBufferSize{ 1024 * 1024 * 8 }; // 8MB

	uint32_t width{ 800 };
	uint32_t height{ 600 };

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	uint32_t frameIndex{ 0 };

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> gbufferRtvHeap;

	uint32_t rtvDescriptorSize{ 0 };
	uint32_t dsvDescriptorSize{ 0 };
	uint32_t srvDescriptorSize{ 0 };
	uint32_t gbufferRtvDescriptorSize{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargets[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12Resource> gbufferAlbedo;
	Microsoft::WRL::ComPtr<ID3D12Resource> gbufferNormal;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> uiPipelineState;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> lightingPipelineState;

	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;
	uint8_t* mappedConstantData{ nullptr };
	uint32_t constantBufferOffset{ 0 };
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> transientUploadBuffers;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	Camera* camera{ nullptr };
	D3D12_GPU_VIRTUAL_ADDRESS cameraCbvAddress{ 0 };
	D3D12_GPU_VIRTUAL_ADDRESS lightCbvAddress{ 0 };

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceValues[SwapChainBufferCount]{ 0 };
	HANDLE fenceEvent{ nullptr };
};

template <class T>
inline D3D12_GPU_VIRTUAL_ADDRESS RenderSystem::UploadConstantsData(const T& data_)
{
	const uint32_t size{ (static_cast<uint32_t>(sizeof(T)) + 255) & ~255 };
	
	// Ensure we don't overflow the current frame's portion of the constant buffer
	// We use frameIndex to segment the buffer: [frame 0][frame 1]
	const uint32_t frameStartOffset{ frameIndex * (MaxConstantBufferSize / SwapChainBufferCount) };
	const uint32_t frameEndOffset{ (frameIndex + 1) * (MaxConstantBufferSize / SwapChainBufferCount) };

	if (constantBufferOffset < frameStartOffset || constantBufferOffset + size > frameEndOffset)
	{
		constantBufferOffset = frameStartOffset;
	}

	if (mappedConstantData != nullptr)
	{
		std::memcpy(mappedConstantData + constantBufferOffset, &data_, sizeof(T));
	}

	const D3D12_GPU_VIRTUAL_ADDRESS gpuAddress{ constantBuffer->GetGPUVirtualAddress() + constantBufferOffset };
	constantBufferOffset += size;

	return gpuAddress;
}
