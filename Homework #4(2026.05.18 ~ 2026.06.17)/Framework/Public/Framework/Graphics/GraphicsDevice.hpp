#pragma once

namespace TUK::Framework
{
	class GraphicsDevice
	{
	public:
		virtual ~GraphicsDevice() noexcept;

		GraphicsDevice(const GraphicsDevice&) = delete;
		GraphicsDevice& operator=(const GraphicsDevice&) = delete;

		GraphicsDevice(GraphicsDevice&&) = delete;
		GraphicsDevice& operator=(GraphicsDevice&&) = delete;

	protected:
		GraphicsDevice() noexcept;
	};
}
