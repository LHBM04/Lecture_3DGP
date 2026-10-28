#pragma once

namespace TUK::Framework
{
	class Display;

	struct GPUSwapChainOptions final
	{
		const Display* display;
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t bufferCount;
	};
}
