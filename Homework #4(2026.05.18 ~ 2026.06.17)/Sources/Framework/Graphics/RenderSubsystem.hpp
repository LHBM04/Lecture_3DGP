#pragma once

#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <dxgi1_6.h>
#include "../Core/EngineSubsystem.hpp"
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "RenderTarget.hpp"
#include "Shader.hpp"
#include "SwapChain.hpp"

namespace TUK::Framework
{
	class Window;
	class WindowSubsystem;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;
		[[nodiscard]] Renderer& GetRenderer() noexcept;
		[[nodiscard]] bool IsFrameActive() const noexcept;
		[[nodiscard]] RenderTarget& GetWindowTarget(Window& window);
		void BlitToWindow(RenderTarget& source, Window& window);
		[[nodiscard]] std::expected<RenderTarget, std::string> CreateRenderTarget(
			UINT width, UINT height, bool withDepth = false, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM) const;
		[[nodiscard]] std::expected<Texture, std::string> CreateTexture(
			UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM) const;
		[[nodiscard]] std::expected<Buffer, std::string> CreateBuffer(UINT64 size, D3D12_HEAP_TYPE heapType,
			D3D12_RESOURCE_STATES initialState, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE) const;
		[[nodiscard]] std::expected<Microsoft::WRL::ComPtr<ID3D12RootSignature>, std::string> CreateRootSignature(
			std::span<const std::byte> serializedSignature) const;
		[[nodiscard]] std::expected<Shader, std::string> CreateShader(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& description) const;
		[[nodiscard]] std::expected<Shader, std::string> CreateComputeShader(const D3D12_COMPUTE_PIPELINE_STATE_DESC& description) const;
		[[nodiscard]] ID3D12Device& GetNativeDevice() const noexcept;

	protected:
		void OnStartup() override;
		void OnPreTick() override;
		void OnPostTick() override;
		void OnShutdown() override;

	private:
		std::expected<void, std::string> InitializeDevice();
		std::expected<void, std::string> InitializeScreenShader();
		std::expected<std::unique_ptr<SwapChain>, std::string> CreateSwapChain(Window& window, UINT width, UINT height);
		std::expected<void, std::string> UpdateWindowTargets(SwapChain& output);
		std::expected<void, std::string> UpdateWindows();
		std::expected<Microsoft::WRL::ComPtr<ID3D12Resource>, std::string> CreateResource(
			const D3D12_RESOURCE_DESC& description, D3D12_HEAP_TYPE heapType,
			D3D12_RESOURCE_STATES initialState, const D3D12_CLEAR_VALUE* clearValue = nullptr) const;
		std::expected<void, std::string> CreateShaderView(Texture& texture) const;
		bool WaitForGpu();
		bool CheckResult(HRESULT result, std::string_view operation);
		void ReportError(std::string_view message);
		template <class T> bool CheckResult(const std::expected<T, std::string>& result)
		{
			if (!result) ReportError(result.error());
			return result.has_value();
		}
		WindowSubsystem* windowSubsystem = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory6> factory{};
		Microsoft::WRL::ComPtr<ID3D12Device> device{};
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue{};
		Microsoft::WRL::ComPtr<ID3D12Fence> fence{};
		HANDLE completionEvent = nullptr;
		UINT64 fenceValue = 0;
		Renderer renderer{};
		Shader screenShader{};
		std::vector<std::unique_ptr<SwapChain>> outputs;
		std::vector<SwapChain*> frameOutputs;
		bool isInitialized = false;
		bool hasFailed = false;
	};
}
