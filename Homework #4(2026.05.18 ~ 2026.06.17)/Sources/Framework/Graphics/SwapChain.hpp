#pragma once

#include <vector>
#include <dxgi1_6.h>
#include <wrl.h>
#include "RenderTarget.hpp"

namespace TUK::Framework
{
	class RenderSubsystem;

	/** 창 핸들, 네이티브 스왑 체인과 백 버퍼 타겟을 소유한다. GPU 동기화는 RenderSubsystem이 담당한다. */
	class SwapChain final
	{
		friend class RenderSubsystem;

	public:
		SwapChain() noexcept = default;
		/** GPU가 백 버퍼 사용을 마친 뒤 해제한다. */
		~SwapChain() noexcept;

		SwapChain(const SwapChain&) = delete;
		SwapChain& operator=(const SwapChain&) = delete;
		SwapChain(SwapChain&&) = delete;
		SwapChain& operator=(SwapChain&&) = delete;

	private:
		HWND handle = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain{};
		std::vector<RenderTarget> targets;
	};
}
