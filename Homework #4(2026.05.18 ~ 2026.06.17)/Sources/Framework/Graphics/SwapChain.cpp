#include "Precompiled.hpp"
#include "SwapChain.hpp"

namespace TUK::Framework
{
	SwapChain::~SwapChain() noexcept
	{
		if (swapChain)
		{
			(void)swapChain->SetFullscreenState(FALSE, nullptr);
		}
	}
}
