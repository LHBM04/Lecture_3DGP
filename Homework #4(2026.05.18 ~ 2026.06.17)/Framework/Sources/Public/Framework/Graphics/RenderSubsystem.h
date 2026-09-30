#pragma once

#include <memory>

#include "../Core/Subsystem.h"

namespace TUK::Framework
{
	class GraphicsDevice;

	class RenderSubsystem : public Subsystem
	{
	public:
		RenderSubsystem() noexcept = default;
		~RenderSubsystem() noexcept override = default;

		void OnStartup() override;
		void OnShutdown() override;

		[[nodiscard]] GraphicsDevice& GetDevice() const noexcept;

	private:
		std::unique_ptr<GraphicsDevice> device;
	};
}
