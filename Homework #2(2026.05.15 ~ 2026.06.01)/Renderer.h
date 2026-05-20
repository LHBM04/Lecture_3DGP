#pragma once

#include "ColorRGB.h"
#include "RendererOptions.h"

class Mesh;

class Renderer
{
	friend class RenderSystem;

public:
	~Renderer() noexcept;

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void Resize(int width_, int height_);

	[[nodiscard]] bool ShouldVSync() const noexcept;
	void SetShouldVSync(bool enabled_) noexcept;

	[[nodiscard]] bool ShouldTearing() const noexcept;
	void SetShouldTearing(bool enabled_) noexcept;

	void PreRender();
	void PostRender();

	void Clear();
	void DrawMesh(const Mesh& mesh_);
	void Present();

private:
	static constexpr UINT bufferCount = 2;

	Renderer() noexcept = default;

	[[nodiscard]] bool Initialize(const RendererOptions& options_);
	[[nodiscard]] bool CreateSwapChain(const RendererOptions& options_);
	[[nodiscard]] bool CreateRenderTargetViews();
	[[nodiscard]] bool CreateFrameResources();
	[[nodiscard]] bool CreateFence();

	void UpdateViewportAndScissor() noexcept;
	void WaitForGPU() noexcept;
	void ReleaseBackBuffers() noexcept;

	RendererOptions options{};
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
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
};
