#include "Precompiled.hpp"
#include "SceneTransition.hpp"

#include "../Core/Assets.hpp"

namespace TUK::Game
{
	void SceneTransition::OnAwake()
	{
		Transition::OnAwake();

		scenes = App::GetInstance().GetSubsystem<SceneSubsystem>();
		input = App::GetInstance().GetSubsystem<InputSubsystem>();

		if (scenes && exitSceneListenerId == 0)
		{
			exitSceneListenerId = scenes->OnExitScene().AddListener([this]([[maybe_unused]] Scene* exitedScene)
			{
				material = nullptr;
			});
		}

		if (!material && !materialPath.empty())
		{
			material = Assets::LoadAsset<MaterialAsset>(materialPath);
		}
	}

	void SceneTransition::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		const f32 opacity = GetProgress();
		if (opacity <= 0.0f)
		{
			return;
		}

		if (!material && !materialPath.empty())
		{
			material = Assets::LoadAsset<MaterialAsset>(materialPath);
		}

		if (!material)
		{
			return;
		}

		const D3D12_RECT& bounds = commandBuffer.GetActiveTargetBounds();
		const CD3DX12_VIEWPORT viewport(
			static_cast<f32>(bounds.left),
			static_cast<f32>(bounds.top),
			static_cast<f32>(bounds.right - bounds.left),
			static_cast<f32>(bounds.bottom - bounds.top));
		commandBuffer.SetViewport(viewport);
		commandBuffer.SetScissorRect(bounds);

		const ColorRGBA<f32> color{
			fadeColor.GetR(),
			fadeColor.GetG(),
			fadeColor.GetB(),
			fadeColor.GetA() * opacity
		};
		commandBuffer.SetGraphicsRootConstants(
			GPUCommandBuffer::ROOT_CONSTANT_SLOT, std::as_bytes(std::span(&color, 1)));

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());
		constexpr u32 FULLSCREEN_VERTEX_COUNT = 3;
		commandBuffer.Draw(FULLSCREEN_VERTEX_COUNT);
	}

	void SceneTransition::OnDestroy()
	{
		if (IsActive() && input)
		{
			input->SetInputEnabled(wasInputEnabled);
		}

		if (scenes && exitSceneListenerId != 0)
		{
			scenes->OnExitScene().RemoveListener(exitSceneListenerId);
			exitSceneListenerId = 0;
		}

		Reset();
		material = nullptr;
		scenes = nullptr;
		input = nullptr;
	}

	void SceneTransition::Reset() noexcept
	{
		Transition::Reset();
		targetScene = nullptr;
	}

	void SceneTransition::OpenComplete()
	{
		if (GetState() == TransitionState::OPEN_COMPLETE)
		{
			return;
		}

		Transition::OpenComplete();
		NextScene();
	}

	void SceneTransition::CloseComplete()
	{
		if (GetState() == TransitionState::CLOSE_COMPLETE)
		{
			return;
		}

		Transition::CloseComplete();
		if (input)
		{
			input->SetInputEnabled(wasInputEnabled);
		}

		targetScene = nullptr;
		Stop();
	}

	bool SceneTransition::Go(Scene* scene, f32 duration)
	{
		if (!scenes || IsActive() || !scene || scene == scenes->GetCurrentScene())
		{
			return false;
		}

		if (!std::isfinite(duration) || duration <= 0.0f)
		{
			return false;
		}

		targetScene = scene;
		SetDuration(duration);
		if (input)
		{
			wasInputEnabled = input->IsInputEnabled();
			input->SetInputEnabled(false);
		}

		Open();
		return true;
	}

	void SceneTransition::NextScene()
	{
		Close();

		if (scenes && targetScene)
		{
			if (!scenes->RequestSceneChange(targetScene))
			{
				LOG_ERROR("페이드 후 월드 전환을 예약하지 못했습니다.");
			}
		}

		onNextScene.Broadcast();
	}
}
