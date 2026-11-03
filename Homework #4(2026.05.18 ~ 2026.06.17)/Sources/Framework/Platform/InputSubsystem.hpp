#pragma once

#include <unordered_map>
#include <memory>

#include "../Core/AppSubsystem.hpp"

namespace TUK::Framework
{
	class Display;
	class InputProvider;

	class InputSubsystem : public AppSubsystem
	{
	public:
		InputSubsystem() noexcept;
		~InputSubsystem() noexcept override;

		void AddProvider(const Display& display);
		void RemoveProvider(const Display& display);

		[[nodiscard]] InputProvider& GetProvider(const Display& display) const noexcept
		{
			return *providers.at(&display);
		}

	protected:
		SystemStatus OnStartup() override;
		SystemStatus OnEarlyUpdate() override;
		SystemStatus OnFixedUpdate() override;
		SystemStatus OnUpdate() override;
		SystemStatus OnLateUpdate() override;
		void OnShutdown() override;

	private:
		std::unordered_map<const Display*, std::unique_ptr<InputProvider>> providers;
	};
}
