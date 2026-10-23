#pragma once

#include <functional>
#include <map>
#include <optional>
#include <vector>

#include "../Core/EngineSubsystem.hpp"

namespace TUK::Framework
{
	class Display;

	class RenderSubsystem : public EngineSubsystem
	{
	public:
		RenderSubsystem() noexcept;
		~RenderSubsystem() noexcept override;

		void RegisterDisplay(const Display& window);
		void UnregisterDisplay(const Display& window);

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnPreTick() override;
		SystemStatus OnPostTick() override;
		void OnShutdown() override;

	private:
		using GraphicsDevice = Microsoft::WRL::ComPtr<ID3D12Device>;
		using SwapChain = Microsoft::WRL::ComPtr<IDXGISwapChain4>;

		void RemovePendingDisplays();

		GraphicsDevice d3dDevice;
		std::unordered_map<const Display*, SwapChain> swapChains;
	};
}
