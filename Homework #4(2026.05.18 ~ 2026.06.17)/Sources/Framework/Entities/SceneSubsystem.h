#pragma once

#include <memory>
#include <vector>

#include "../Core/Subsystem.h"

namespace TUK::Framework
{
	class Scene;

	class SceneSubsystem : public Subsystem
	{
	public:
		SceneSubsystem() noexcept;
		~SceneSubsystem() noexcept override;

	protected:
		void OnStartup() override;
		void OnTick() override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<Scene>> scenes;
	};
}
