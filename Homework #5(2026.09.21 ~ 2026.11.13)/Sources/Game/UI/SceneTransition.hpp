#pragma once

#include "Framework/Graphics.hpp"
#include "Framework/Platform.hpp"
#include "Framework/Resources.hpp"

#include "Transition.hpp"

namespace TUK::Game
{
	class SceneTransition : public Transition
	{
	public:
		explicit SceneTransition(GameObject* owner) noexcept;
		~SceneTransition() noexcept override = default;

		void OnAwake() override;
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;
		void OnDestroy() override;

		void Reset() noexcept override;
		void OpenComplete() override;
		void CloseComplete() override;

		[[nodiscard]] bool Go(Scene* scene, f32 duration = 0.35f);
		virtual void NextScene();

		[[nodiscard]] bool IsActive() const noexcept;
		[[nodiscard]] Scene* GetTargetScene() const noexcept;
		void SetTargetScene(Scene* scene) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetFadeColor() const noexcept;
		void SetFadeColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] Delegate<>& OnNextScene() noexcept;

	private:
		static constexpr std::string_view DEFAULT_MATERIAL_PATH = "Resources/Materials/SceneTransition.tukmat";

		SceneSubsystem* scenes = nullptr;
		InputSubsystem* input = nullptr;
		const MaterialAsset* material = nullptr;
		std::filesystem::path materialPath = DEFAULT_MATERIAL_PATH;
		ColorRGBA<f32> fadeColor{ 0.0f, 0.0f, 0.0f, 1.0f };
		Scene* targetScene = nullptr;
		bool wasInputEnabled = true;

		Delegate<Scene*>::ListenerId exitSceneListenerId = 0;
		Delegate<> onNextScene;
	};

	inline SceneTransition::SceneTransition(GameObject* owner) noexcept
		: Transition(owner)
	{
	}

	inline bool SceneTransition::IsActive() const noexcept
	{
		return GetState() != TransitionState::IDLE && GetState() != TransitionState::CLOSE_COMPLETE;
	}

	inline Scene* SceneTransition::GetTargetScene() const noexcept
	{
		return targetScene;
	}

	inline void SceneTransition::SetTargetScene(Scene* scene) noexcept
	{
		targetScene = scene;
	}

	inline const MaterialAsset* SceneTransition::GetMaterial() const noexcept
	{
		return material;
	}

	inline void SceneTransition::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
		if (material)
		{
			materialPath = material->GetPath();
		}
	}

	inline const ColorRGBA<f32>& SceneTransition::GetFadeColor() const noexcept
	{
		return fadeColor;
	}

	inline void SceneTransition::SetFadeColor(const ColorRGBA<f32>& value) noexcept
	{
		fadeColor = value;
	}

	inline Delegate<>& SceneTransition::OnNextScene() noexcept
	{
		return onNextScene;
	}
}
