#include "Precompiled.hpp"
#include "SceneTransition.hpp"

#include <cmath>

#include "Framework/GameObjects/Scene.hpp"
#include "Framework/GameObjects/SceneSubsystem.hpp"
#include "Framework/Platform/InputSubsystem.hpp"

namespace TUK::Game
{
	using namespace Framework;

	std::expected<void, std::string> SceneTransition::Initialize(
		SceneSubsystem* scenes, InputSubsystem* input, const GPUDevice& device, AssetSubsystem* assets)
	{
		ASSERT_FMT(scenes, "월드 서브시스템이 nullptr입니다.");
		ASSERT_FMT(input, "입력 서브시스템이 nullptr입니다.");
		if (auto initialized = transition.Initialize(device, assets); !initialized)
		{
			return initialized;
		}
		this->scenes = scenes;
		this->input = input;
		return {};
	}

	void SceneTransition::Shutdown() noexcept
	{
		Finish();
		transition.Shutdown();
		scenes = nullptr;
		input = nullptr;
	}

	bool SceneTransition::Request(Scene* scene, f32 duration) noexcept
	{
		if (!scenes || !input || IsActive() || !scene || scene == scenes->GetCurrentScene())
		{
			return false;
		}
		if (!transition.Exit(duration))
		{
			return false;
		}
		targetScene = scene;
		fadeDuration = duration;
		wasInputEnabled = input->IsInputEnabled();
		input->SetInputEnabled(false);
		phase = Phase::WAIT_FOR_EXIT;
		return true;
	}

	void SceneTransition::Update(f32 deltaTime)
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
					if (scenes->RequestSceneChange(targetScene))
					{
						phase = Phase::WAIT_FOR_SCENE;
					}
					else
					{
						LOG_ERROR("페이드 후 월드 전환을 예약하지 못했습니다.");
						BeginEnter();
					}
				}
				break;
			}
			case Phase::WAIT_FOR_SCENE:
			{
				const Scene* currentScene = scenes->GetCurrentScene();
				if (currentScene && currentScene->IsEntered())
				{
					if (currentScene != targetScene)
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

	void SceneTransition::BeginEnter() noexcept
	{
		if (!transition.Enter(fadeDuration))
		{
			Finish();
			return;
		}
		phase = Phase::WAIT_FOR_ENTER;
	}

	void SceneTransition::Finish() noexcept
	{
		if (IsActive())
		{
			input->SetInputEnabled(wasInputEnabled);
		}
		phase = Phase::IDLE;
		targetScene = nullptr;
		transition.Reset();
	}
}
