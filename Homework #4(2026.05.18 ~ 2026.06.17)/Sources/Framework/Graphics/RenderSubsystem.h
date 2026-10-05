#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

#include <string_view>
#include <cstddef>
#include <vector>
#include <functional>

#include "../Core/Subsystem.h"
#include "SwapChain.h"
#include "Renderer.h"

namespace TUK::Framework
{
	class WindowSubsystem;

	class RenderSubsystem : public Subsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		/** 디바이스 가져오기 */
		[[nodiscard]] ID3D12Device& GetDevice() const noexcept;
		[[nodiscard]] Renderer& GetRenderer() noexcept;
		/** 현재 프레임의 활성 스왑 체인. 반환한 참조는 다음 OnPreTick 이전까지만 사용한다. */
		[[nodiscard]] std::expected<std::reference_wrapper<SwapChain>, std::string> GetSwapChain(Window& window);

	protected:
		void OnStartup() override;
		void OnPreTick() override;
		void OnPostTick() override;
		void OnShutdown() override;

	private:
		bool CheckResult(HRESULT result, std::string_view operation);
		bool CheckResult(const std::expected<void, std::string>& result);
		bool InitializeDevice();
		bool WaitForGpu();

		WindowSubsystem* windowSubsystem;
		Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		Renderer renderer;
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		HANDLE fenceEvent;
		UINT64 fenceValue;
		std::vector<SwapChain> swapChains;
		std::vector<std::size_t> frameSwapChains;
		bool isFrameRecording;
		bool isInitialized;
		bool hasFailed;
	};
}
