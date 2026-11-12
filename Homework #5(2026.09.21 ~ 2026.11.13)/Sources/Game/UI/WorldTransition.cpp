#include "Precompiled.hpp"
#include "WorldTransition.hpp"

#include <cmath>

#include "Framework/Objects/World.hpp"
#include "Framework/Objects/WorldSubsystem.hpp"
#include "Framework/Platform/InputSubsystem.hpp"

namespace TUK::Game
{
	using namespace Framework;

	std::expected<void, std::string> WorldTransition::Initialize(
		WorldSubsystem* worlds, InputSubsystem* input, const GPUDevice& device, AssetSubsystem* assets)
	{
		ASSERT_FMT(worlds, "월드 서브시스템이 nullptr입니다.");
		ASSERT_FMT(input, "입력 서브시스템이 nullptr입니다.");
		if (auto initialized = transition.Initialize(device, assets); !initialized)
		{
			return initialized;
		}
		this->worlds = worlds;
		this->input = input;
		return {};
	}

	void WorldTransition::Shutdown() noexcept
	{
		Finish();
		transition.Shutdown();
		worlds = nullptr;
		input = nullptr;
	}

	bool WorldTransition::Request(World* world, f32 duration) noexcept
	{
		if (!worlds || !input || IsActive() || !world || world == worlds->GetCurrentWorld())
		{
			return false;
		}
		if (!transition.Exit(duration))
		{
			return false;
		}
		targetWorld = world;
		fadeDuration = duration;
		wasInputEnabled = input->IsInputEnabled();
		input->SetInputEnabled(false);
		phase = Phase::WAIT_FOR_EXIT;
		return true;
	}

	void WorldTransition::Update(f32 deltaTime)
	{
		if (!std::isfinite(deltaTime) || deltaTime < 0.0f)
		{
			return;
		}
		transition.Update(deltaTime);
		switch (phase)
		{
			case Phase::WAIT_FOR_EXIT:
			{
				// The next update follows submission and presentation of the covered frame.
				if (transition.HasRenderedExit())
				{
					if (worlds->RequestWorldChange(targetWorld))
					{
						phase = Phase::WAIT_FOR_WORLD;
					}
					else
					{
						LOG_ERROR("페이드 후 월드 전환을 예약하지 못했습니다.");
						BeginEnter();
					}
				}
				break;
			}
			case Phase::WAIT_FOR_WORLD:
			{
				const World* currentWorld = worlds->GetCurrentWorld();
				if (currentWorld && currentWorld->IsEntered())
				{
					if (currentWorld != targetWorld)
					{
						LOG_WARNING("페이드 중 전환 대상이 변경되어 현재 월드를 표시합니다.");
					}
					BeginEnter();
				}
				break;
			}
			case Phase::WAIT_FOR_ENTER:
			{
				if (transition.GetCurrentState() == TransitionState::ENTERCOMPLETE)
				{
					Finish();
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void WorldTransition::BeginEnter() noexcept
	{
		if (!transition.Enter(fadeDuration))
		{
			Finish();
			return;
		}
		phase = Phase::WAIT_FOR_ENTER;
	}

	void WorldTransition::Finish() noexcept
	{
		if (IsActive())
		{
			input->SetInputEnabled(wasInputEnabled);
		}
		phase = Phase::IDLE;
		targetWorld = nullptr;
		transition.Reset();
	}
}
