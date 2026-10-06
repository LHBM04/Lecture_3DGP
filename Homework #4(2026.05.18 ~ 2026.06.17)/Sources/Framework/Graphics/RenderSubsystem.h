#pragma once

#include <expected>
#include <string>

#include <d3d12.h>
#include <dxgi1_6.h>

#include <wrl.h>

#include <string_view>
#include <cstddef>
#include <vector>
#include <functional>

#include "../Core/EngineSubsystem.h"
#include "SwapChain.h"
#include "RenderContext.h"
#include "GraphicsDevice.h"
#include "Fence.h"

namespace TUK::Framework
{
	class WindowSubsystem;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		/** 디바이스 가져오기 */
		[[nodiscard]] GraphicsDevice& GetDevice() noexcept;
		[[nodiscard]] RenderContext& GetRenderContext() noexcept;
		/** 해당 창에 현재 프레임의 활성 스왑 체인이 있어야 한다. 반환한 참조는 다음 OnPreTick 이전까지만 사용한다. */
		[[nodiscard]] SwapChain& GetSwapChain(Window& window);

	protected:
		void OnStartup() override;
		void OnPreTick() override;
		void OnPostTick() override;
		void OnShutdown() override;

	private:
		bool CheckResult(HRESULT result, std::string_view operation);
		void ReportError(std::string_view message);

		template <class T>
		bool CheckResult(const std::expected<T, std::string>& result);
		bool WaitForGpu();

		WindowSubsystem* windowSubsystem;
		GraphicsDevice device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		RenderContext renderContext;
		Fence fence;
		std::vector<SwapChain> swapChains;
		std::vector<std::size_t> frameSwapChains;
		bool isFrameRecording;
		bool isInitialized;
		bool hasFailed;
	};

	template <class T>
	bool RenderSubsystem::CheckResult(const std::expected<T, std::string>& result)
	{
		if (!result)
		{
			ReportError(result.error());
		}
		return result.has_value();
	}
}
