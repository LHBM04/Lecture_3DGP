#include "Precompiled.hpp"
#include "WorldSubsystem.hpp"

#include <cstdlib>
#include <iostream>

#include "../Graphics/Camera.hpp"
#include "../Graphics/GPUCommandBuffer.hpp"

#include "Component.hpp"
#include "Entity.hpp"
#include "World.hpp"

namespace TUK::Framework
{
	WorldSubsystem::~WorldSubsystem() noexcept
	{
		worlds.clear();
	}

	World& WorldSubsystem::AddWorld(std::unique_ptr<World> world)
	{
		if (!world || currentWorld)
		{
			std::abort();
		}

		World& result = *world;
		worlds.push_back(std::move(world));
		return result;
	}

	void WorldSubsystem::ExitWorld()
	{
		if (!currentWorld)
		{
			return;
		}

		World* exitingWorld = currentWorld;
		currentWorld = nullptr;

		if (exitingWorld->IsEntered())
		{
			exitingWorld->SetEntered(false);
			exitingWorld->OnExit();
		}
		exitingWorld->ClearEntities();
	}

	void WorldSubsystem::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (!currentWorld || !currentWorld->IsEntered())
		{
			return;
		}

		const usize entityCount = currentWorld->GetEntities().size();
		for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
		{
			Entity& entity = *currentWorld->GetEntities()[entityIndex];
			if (!entity.IsEnabled() || entity.IsDestroyed())
			{
				continue;
			}

			const usize componentCount = entity.GetComponents().size();
			for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				Component& component = *entity.GetComponents()[componentIndex];
				if (component.IsStarted() && component.IsEnabled() && !component.IsDestroyed())
				{
					component.OnPreRender(commandBuffer);
				}
			}
		}
	}

	void WorldSubsystem::Render(GPUCommandBuffer& commandBuffer)
	{
		if (!currentWorld || !currentWorld->IsEntered())
		{
			return;
		}

		for (Camera* camera : currentWorld->GetCameras())
		{
			if (!camera || !camera->IsEnabled() || camera->IsDestroyed())
			{
				continue;
			}

			const Entity& cameraOwner = camera->GetOwner();
			if (!cameraOwner.IsEnabled() || cameraOwner.IsDestroyed())
			{
				continue;
			}

			if (auto result = commandBuffer.SetCamera(*camera); !result)
			{
				std::cerr << "WorldSubsystem 카메라 설정 실패: " << result.error() << std::endl;
				continue;
			}

			const usize entityCount = currentWorld->GetEntities().size();
			for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
			{
				Entity& entity = *currentWorld->GetEntities()[entityIndex];
				if (!entity.IsEnabled() || entity.IsDestroyed())
				{
					continue;
				}

				const usize componentCount = entity.GetComponents().size();
				for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
				{
					Component& component = *entity.GetComponents()[componentIndex];
					if (component.IsStarted() && component.IsEnabled() && !component.IsDestroyed())
					{
						component.OnRender(commandBuffer);
					}
				}
			}
		}
	}

	void WorldSubsystem::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!currentWorld || !currentWorld->IsEntered())
		{
			return;
		}

		const usize entityCount = currentWorld->GetEntities().size();
		for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
		{
			Entity& entity = *currentWorld->GetEntities()[entityIndex];
			if (!entity.IsEnabled() || entity.IsDestroyed())
			{
				continue;
			}

			const usize componentCount = entity.GetComponents().size();
			for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				Component& component = *entity.GetComponents()[componentIndex];
				if (component.IsStarted() && component.IsEnabled() && !component.IsDestroyed())
				{
					component.OnPostRender(commandBuffer);
				}
			}
		}
	}

	SystemStatus WorldSubsystem::OnStartup()
	{
		if (worlds.empty())
		{
			return SystemStatus::FAILURE;
		}

		currentWorld = worlds.front().get();
		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnEarlyUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::FAILURE;
		}

		if (!currentWorld->IsEntered())
		{
			currentWorld->SetEntered(true);
			currentWorld->OnEnter();
		}

		if (currentWorld)
		{
			currentWorld->DestroyEntities();
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::CONTINUE;
		}

		currentWorld->DestroyEntities();
		currentWorld->OnFixedUpdate();

		if (!currentWorld)
		{
			return SystemStatus::CONTINUE;
		}

		const usize entityCount = currentWorld->GetEntities().size();
		for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
		{
			Entity& entity = *currentWorld->GetEntities()[entityIndex];
			if (!entity.IsEnabled() || entity.IsDestroyed())
			{
				continue;
			}

			const usize componentCount = entity.GetComponents().size();
			for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				Component& component = *entity.GetComponents()[componentIndex];
				if (!component.IsEnabled() || component.IsDestroyed())
				{
					continue;
				}

				if (!component.IsStarted())
				{
					component.SetStarted(true);
					component.OnStart();
					if (!component.IsEnabled() || component.IsDestroyed() || !entity.IsEnabled() || entity.IsDestroyed())
					{
						continue;
					}
				}

				component.OnFixedUpdate();
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::CONTINUE;
		}

		currentWorld->DestroyEntities();
		currentWorld->OnUpdate();

		if (!currentWorld)
		{
			return SystemStatus::CONTINUE;
		}

		const usize entityCount = currentWorld->GetEntities().size();
		for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
		{
			Entity& entity = *currentWorld->GetEntities()[entityIndex];
			if (!entity.IsEnabled() || entity.IsDestroyed())
			{
				continue;
			}

			const usize componentCount = entity.GetComponents().size();
			for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				Component& component = *entity.GetComponents()[componentIndex];
				if (!component.IsEnabled() || component.IsDestroyed())
				{
					continue;
				}

				if (!component.IsStarted())
				{
					component.SetStarted(true);
					component.OnStart();
					if (!component.IsEnabled() || component.IsDestroyed() || !entity.IsEnabled() || entity.IsDestroyed())
					{
						continue;
					}
				}

				component.OnUpdate();
			}
		}

		return SystemStatus::CONTINUE;
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::SUCCESS;
		}

		currentWorld->DestroyEntities();
		currentWorld->OnLateUpdate();

		if (!currentWorld)
		{
			return SystemStatus::SUCCESS;
		}

		const usize entityCount = currentWorld->GetEntities().size();
		for (usize entityIndex = 0; entityIndex < entityCount; ++entityIndex)
		{
			Entity& entity = *currentWorld->GetEntities()[entityIndex];
			if (!entity.IsEnabled() || entity.IsDestroyed())
			{
				continue;
			}

			const usize componentCount = entity.GetComponents().size();
			for (usize componentIndex = 0; componentIndex < componentCount; ++componentIndex)
			{
				Component& component = *entity.GetComponents()[componentIndex];
				if (!component.IsEnabled() || component.IsDestroyed())
				{
					continue;
				}

				if (!component.IsStarted())
				{
					component.SetStarted(true);
					component.OnStart();
					if (!component.IsEnabled() || component.IsDestroyed() || !entity.IsEnabled() || entity.IsDestroyed())
					{
						continue;
					}
				}

				component.OnLateUpdate();
			}
		}

		if (!currentWorld)
		{
			return SystemStatus::SUCCESS;
		}

		return SystemStatus::CONTINUE;
	}

	void WorldSubsystem::OnShutdown()
	{
		ExitWorld();
		worlds.clear();
	}
}
