#pragma once

namespace TUK::Framework
{
	class Display;

	struct SwapChainCreateInfo final
	{
		Display& window;
		std::uint32_t bufferCount;
		std::uint32_t width;
		std::uint32_t height;
	};
}
