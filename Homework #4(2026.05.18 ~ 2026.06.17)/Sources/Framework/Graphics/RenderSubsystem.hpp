#pragma once

#include <optional>

#include "../Core/EngineSubsystem.hpp"

#include "GPUDevice.hpp"

namespace TUK::Framework
{
	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		std::expected<void, std::string> InitializeGPUDevice();
		
		std::optional<GPUDevice> device;
	};
}
