#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "RenderContext.h"
#include "Renderer.h"
#include "RendererOptions.h"
#include "System.h"

class Window;

class RenderSystem final : public ISystem
{
public:
	bool Initialize();
	void Release() override;

	void BeginRender();
	void EndRender();
	void ExecuteRenderer(Window* window_, Renderer* renderer_, const RenderContext& context_);

	std::expected<Renderer*, std::wstring> CreateRenderer();
	std::expected<Renderer*, std::wstring> CreateRenderer(const RendererOptions& options_);
	void DestroyRenderer(Renderer* renderer_);

	std::expected<RenderContext*, std::wstring> CreateRenderContext();
	void DestroyRenderContext(RenderContext* context_);

	std::expected<void, std::wstring> CreateRenderTarget(Window* window_, int width_, int height_);
	void DestroyRenderTarget(Window* window_);

private:
	struct RenderTarget final
	{
		static constexpr uint32_t BufferCount = 2;

		HWND windowHandle{ nullptr };
		uint32_t width{ 0 };
		uint32_t height{ 0 };
		Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[BufferCount]{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer{ nullptr };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[BufferCount]{};
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
		uint32_t currentBackBufferIndex{ 0 };
	};

	struct FrameResource final
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator{ nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer{ nullptr };
		std::byte* mappedCameraData{ nullptr };
		std::byte* mappedInstanceData{ nullptr };
		uint64_t fenceValue{ 0 };
	};

	struct WindowTargetPair final
	{
		Window* window{ nullptr };
		RenderTarget target{};
	};

	static constexpr uint32_t FrameCount = RenderTarget::BufferCount;

	[[nodiscard]] WindowTargetPair* FindWindowTargetPair(Window* window_) noexcept;
	[[nodiscard]] const WindowTargetPair* FindWindowTargetPair(Window* window_) const noexcept;
	void WaitForFrame(FrameResource& frameResource_);
	void WaitForGpu();

private:
#if defined (_DEBUG)
	Microsoft::WRL::ComPtr<ID3D12Debug> debugController{ nullptr };
#endif

	Microsoft::WRL::ComPtr<IDXGIFactory4> factory{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Device> device{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Fence> fence{ nullptr };

	HANDLE fenceEvent{ nullptr };
	uint64_t nextFenceValue{ 1 };
	uint32_t frameIndex{ 0 };
	uint32_t rtvDescriptorSize{ 0 };
	uint32_t dsvDescriptorSize{ 0 };
	uint32_t allocatedRtvCount{ 0 };
	uint32_t allocatedDsvCount{ 0 };

	std::vector<FrameResource> frameResources;
	std::vector<WindowTargetPair> m_windowTargetPairs;
	std::vector<std::unique_ptr<Renderer>> renderers;
	std::vector<std::unique_ptr<RenderContext>> renderContexts;
	Window* activeWindow{ nullptr };
};
