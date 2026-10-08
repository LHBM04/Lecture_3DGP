#pragma once

#include <memory>
#include <vector>

#include "../Core/GameSubsystem.hpp"

namespace TUK::Framework
{
	class Scene;

	class SceneSubsystem : public GameSubsystem
	{
	public:
		SceneSubsystem() noexcept;
		~SceneSubsystem() noexcept override;

	protected:
		void OnStartup() override;
		void Update(double deltaTime) override;
		void OnShutdown() override;

	private:
		std::vector<std::unique_ptr<Scene>> scenes;
	};
}
