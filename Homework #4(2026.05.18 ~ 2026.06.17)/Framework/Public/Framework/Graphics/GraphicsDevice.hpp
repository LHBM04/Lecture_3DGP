#pragma once

namespace TUK::Framework
{
	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() noexcept = default;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;
		
		GraphicsDevice(GraphicsDevice&&) noexcept = default;
		GraphicsDevice& operator=(GraphicsDevice&&) noexcept = default;

	protected:
		GraphicsDevice() noexcept = default;
	};
}
