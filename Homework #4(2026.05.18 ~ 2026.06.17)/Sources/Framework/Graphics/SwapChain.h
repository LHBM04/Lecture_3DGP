#pragma once

#include <array>
#include <expected>
#include <functional>
#include <string>
#include <vector>

#include <d3d12.h>

#include <dxgi1_6.h>

#include <wrl.h>

namespace TUK::Framework
{
	class Window;
	class RenderContext;
	class GraphicsDevice;

	class SwapChain
	{
		friend class RenderContext;
		friend class GraphicsDevice;

	public:
		explicit SwapChain(Window& window) noexcept;
		~SwapChain() noexcept = default;

		/** 복사 금지 */
		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;

		SwapChain(SwapChain&& other) noexcept;
		SwapChain& operator=(SwapChain&& other) noexcept;

		/** 창이 살아 있는 동안만 사용한다. WindowSubsystem이 창을 소유한다. */
		[[nodiscard]] Window& GetWindow() const noexcept;
		/** 창 삭제 후에도 비교에 사용할 수 있는 생성 당시 핸들. */
		[[nodiscard]] HWND GetHWND() const noexcept;

		/** 초기화 완료, 양수 크기, GPU의 백 버퍼 사용 완료가 필요하다. */
		void Resize(UINT width, UINT height);
		[[nodiscard]] std::expected<void, std::string> Present(UINT syncInterval = 1);

		[[nodiscard]] int GetPositionX() const noexcept;
		void SetPositionX(int x) noexcept;
		[[nodiscard]] int GetPositionY() const noexcept;
		void SetPositionY(int y) noexcept;

		[[nodiscard]] UINT GetSizeX() const noexcept;
		void SetSizeX(UINT width);

		[[nodiscard]] UINT GetSizeY() const noexcept;
		void SetSizeY(UINT height);

	private:
		[[nodiscard]] std::expected<void, std::string> Initialize(ID3D12Device& device, IDXGIFactory6& factory,
			ID3D12CommandQueue& queue, UINT width, UINT height);
		void Clear(ID3D12GraphicsCommandList& commandList, const std::array<float, 4>& color);
		void Bind(ID3D12GraphicsCommandList& commandList) const;
		void EndRender(ID3D12GraphicsCommandList& commandList) const;
		UINT GetCurrentBufferIndex() const noexcept;
		std::expected<void, std::string> CreateRenderTargets();

		std::reference_wrapper<Window> window;
		HWND windowHandle;
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetHeap;
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> buffers;
		UINT descriptorSize;
		int positionX;
		int positionY;
		UINT sizeX;
		UINT sizeY;
	};
}
