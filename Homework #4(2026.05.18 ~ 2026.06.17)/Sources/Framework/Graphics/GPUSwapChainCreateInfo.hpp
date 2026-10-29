#pragma once

namespace TUK::Framework
{
	class Display;

	struct GPUSwapChainCreateInfo final
	{
		const Display* display;
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t bufferCount;
	};
}
