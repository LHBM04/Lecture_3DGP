#include "Precompiled.hpp"
#include "WorldSubsystem.hpp"

#include "World.hpp"

namespace TUK::Framework
{
	WorldSubsystem::WorldSubsystem() noexcept
		: AppSubsystem(20)
		, worlds()
	{
	}

	WorldSubsystem::~WorldSubsystem() noexcept
	{
		worlds.clear();
	}

	SystemStatus WorldSubsystem::OnStartup()
	{
		if (worlds.empty())
		{
			return SystemStatus::Failure;
		}

		currentWorld = worlds.front().get();
		return SystemStatus::Continue;
	}

	SystemStatus WorldSubsystem::OnEarlyUpdate()
	{
		if (!currentWorld)
		{
			return SystemStatus::Failure;
		}

		// 첫 엔진 Tick에서 창 생성 이벤트와 스왑 체인 생성이 처리된 뒤 진입합니다.
		// World::Enter는 이미 진입한 월드에는 아무 작업도 하지 않습니다.
		currentWorld->Enter();
		return SystemStatus::Continue;
	}

	World& WorldSubsystem::AddWorld(std::unique_ptr<World> world)
	{
		if (!world)
		{
			throw std::invalid_argument("World must not be null.");
		}
		if (currentWorld)
		{
			throw std::logic_error("Register worlds before startup.");
		}
		World& result = *world;
		worlds.push_back(std::move(world));
		return result;
	}

	void WorldSubsystem::ExitWorld()
	{
		if (currentWorld)
		{
			currentWorld->Exit();
			currentWorld = nullptr;
		}
	}

	void WorldSubsystem::PreRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->PreRender(commandBuffer);
		}
	}

	void WorldSubsystem::Render(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->Render(commandBuffer);
		}
	}

	void WorldSubsystem::PostRender(GPUCommandBuffer& commandBuffer)
	{
		if (currentWorld)
		{
			currentWorld->PostRender(commandBuffer);
		}
	}

	void WorldSubsystem::OnShutdown()
	{
		if (currentWorld)
		{
			currentWorld->Exit();
		}
		currentWorld = nullptr;
		worlds.clear();
	}

	SystemStatus WorldSubsystem::OnFixedUpdate()
	{
		if (currentWorld)
		{
			currentWorld->FixedUpdate();
		}

		return SystemStatus::Continue;
	}

	SystemStatus WorldSubsystem::OnUpdate()
	{
		if (currentWorld)
		{
			currentWorld->Update();
		}

		return SystemStatus::Continue;
	}

	SystemStatus WorldSubsystem::OnLateUpdate()
	{
		if (currentWorld)
		{
			currentWorld->LateUpdate();
		}

		if (!currentWorld)
		{
			return SystemStatus::Success;
		}

		return SystemStatus::Continue;
	}
}
