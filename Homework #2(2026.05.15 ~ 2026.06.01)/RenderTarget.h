#pragma once

#include "ColorRGB.h"
#include "ColorRGBA.h"
#include "Matrix4x4.h"
#include "RenderTargetOptions.h"

class Camera;
class Material;
class Mesh;
class RectTransform;

class RenderTarget
{
	friend class Renderer;

public:
	~RenderTarget() noexcept;

	RenderTarget(const RenderTarget&) = delete;
	RenderTarget& operator=(const RenderTarget&) = delete;

	RenderTarget(RenderTarget&&) = delete;
	RenderTarget& operator=(RenderTarget&&) = delete;

	void Resize(int width_, int height_);

	[[nodiscard]] bool ShouldVSync() const noexcept;
	void SetShouldVSync(bool enabled_) noexcept;

	[[nodiscard]] bool ShouldTearing() const noexcept;
	void SetShouldTearing(bool enabled_) noexcept;

	void PreRender();
	void PostRender();

	void Clear();
	void SetCamera(const Camera& camera_);
	void SetObject(const Matrix4x4& worldMatrix_);
	void SetMaterial(const Material& material_);
	[[nodiscard]] float GetAspectRatio() const noexcept;
	[[nodiscard]] int GetWidth() const noexcept;
	[[nodiscard]] int GetHeight() const noexcept;
	void DrawMesh(const Mesh& mesh_);
	void DrawMeshInstanced(const Mesh& mesh_, std::span<const Matrix4x4> worldMatrices_);
	void DrawUIRect(const RectTransform& rectTransform_, const ColorRGBA& color_);
	void DrawUIRect(const RectTransform& rectTransform_, const ColorRGBA& color_, const Material& material_);
	void DrawUIRectPixels(float left_, float top_, float width_, float height_, const ColorRGBA& color_);
	void DrawUIRectPixels(float left_, float top_, float width_, float height_, const ColorRGBA& color_, const Material& material_);
	void Present();

private:
	static constexpr UINT bufferCount = 2;

	RenderTarget() noexcept = default;

	[[nodiscard]] bool Initialize(const RenderTargetOptions& options_);
	[[nodiscard]] bool CreateSwapChain(const RenderTargetOptions& options_);
	[[nodiscard]] bool CreateRenderTargetViews();
	[[nodiscard]] bool CreateDepthStencilBuffer();
	[[nodiscard]] bool CreateFrameResources();
	[[nodiscard]] bool CreateFence();

	void UpdateViewportAndScissor() noexcept;
	void WaitForGPU() noexcept;
	void ReleaseBackBuffers() noexcept;
	void ReleaseDepthStencilBuffer() noexcept;
	void SetCameraMatrices(const Matrix4x4& viewMatrix_, const Matrix4x4& projectionMatrix_);
	void DrawUIRectPixelsInternal(
		float left_,
		float top_,
		float width_,
		float height_,
		const ColorRGBA& color_,
		ID3D12PipelineState* pipelineState_);

	RenderTargetOptions options{};
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> depthStencilViewHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadResources;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent{ nullptr };
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	UINT renderTargetViewDescriptorSize{ 0 };
	UINT frameIndex{ 0 };
	UINT64 fenceValue{ 0 };
	bool isVSyncEnabled{ true };
	bool isTearingEnabled{ false };
	bool isRecording{ false };
	ColorRGB clearColor{ 0.08f, 0.10f, 0.14f };
	Matrix4x4 currentObjectMatrix{ Matrix4x4::GetIdentity() };
};
