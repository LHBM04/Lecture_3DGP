#pragma once

#include <array>
#include <functional>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace TUK::Framework
{
	class Window;
	class Renderer;

	class RenderTarget
	{
		friend class Renderer;

	public:
		explicit RenderTarget(Window& window) noexcept;
		~RenderTarget() noexcept = default;

		/** 복사 금지 */
		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

		[[nodiscard]] HRESULT Initialize(ID3D12Device& device, IDXGIFactory6& factory,
			ID3D12CommandQueue& queue, UINT width, UINT height);

		/** 창이 살아 있는 동안만 사용한다. WindowSubsystem이 창을 소유한다. */
		[[nodiscard]] Window& GetWindow() const noexcept;
		/** 창 삭제 후에도 비교에 사용할 수 있는 생성 당시 핸들. */
		[[nodiscard]] HWND GetHWND() const noexcept;

		/** GPU가 백 버퍼 사용을 마친 뒤 호출. */
		[[nodiscard]] HRESULT Resize(UINT width, UINT height);
		[[nodiscard]] HRESULT Present(UINT syncInterval = 1);

		[[nodiscard]] int GetPositionX() const noexcept;
		void SetPositionX(int x) noexcept;
		[[nodiscard]] int GetPositionY() const noexcept;
		void SetPositionY(int y) noexcept;

		[[nodiscard]] UINT GetSizeX() const noexcept;
		[[nodiscard]] HRESULT SetSizeX(UINT width);

		[[nodiscard]] UINT GetSizeY() const noexcept;
		[[nodiscard]] HRESULT SetSizeY(UINT height);

	private:
		void Clear(ID3D12GraphicsCommandList& commandList, const std::array<float, 4>& color);
		void Bind(ID3D12GraphicsCommandList& commandList) const;
		void EndRender(ID3D12GraphicsCommandList& commandList) const;
		void AssertInitialized() const noexcept;
		UINT GetCurrentBufferIndex() const noexcept;
		HRESULT CreateRenderTargets();

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
