#include "Precompiled.h"
#include "SceneManager.h"

#include "Application.h"
#include "ColorRGB.h"
#include "DrawCall.h"
#include "InputManager.h"
#include "Material.h"
#include "Mathf.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"
#include "Timer.h"

namespace
{
	enum class TransitionState : unsigned char
	{
		Idle,
		FadeOut,
		FadeIn
	};

	std::unordered_map<std::wstring, std::unique_ptr<Scene>> scenes;
	Scene* currentScene;
	std::wstring pendingSceneName;
	TransitionState transitionState;
	float transitionDuration;
	float transitionTimer;
	float fadeAlpha;

	std::unique_ptr<Mesh> fadeMesh;
	std::unique_ptr<Shader> fadeShader;
	std::unique_ptr<Material> fadeMaterial;

	Matrix4x4 GetFullscreenClipMatrix() noexcept
	{
		return Matrix4x4(
			2.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -2.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f);
	}

	uint64_t BuildSortKey(uint64_t pipelineId_, uint64_t materialId_, uint64_t meshId_) noexcept
	{
		return ((pipelineId_ & 0xFFFFull) << 48) |
			((materialId_ & 0xFFFFFFull) << 24) |
			(meshId_ & 0xFFFFFFull);
	}

	void LoadSceneImmediately(Scene* scene_)
	{
		if (nullptr != currentScene)
		{
			currentScene->Unload();
		}

		currentScene = scene_;
		if (nullptr != currentScene)
		{
			currentScene->Load();
		}
	}

	void StartTransition(const std::wstring& name_)
	{
		pendingSceneName = name_;
		transitionState = TransitionState::FadeOut;
		transitionTimer = 0.0f;
		fadeAlpha = 0.0f;
	}

	void SwapToPendingScene()
	{
		auto iterator{ scenes.find(pendingSceneName) };
		if (scenes.end() == iterator)
		{
			pendingSceneName.clear();
			transitionState = TransitionState::FadeIn;
			transitionTimer = 0.0f;
			return;
		}

		LoadSceneImmediately(iterator->second.get());
		pendingSceneName.clear();
		transitionState = TransitionState::FadeIn;
		transitionTimer = 0.0f;
		fadeAlpha = 1.0f;
	}

	void UpdateTransition()
	{
		if (TransitionState::Idle == transitionState)
		{
			return;
		}

		const float duration{ std::max(0.01f, transitionDuration) };
		transitionTimer += Timer::GetUnscaledDeltaTime();
		const float normalized{ Mathf::Clamp(transitionTimer / duration, 0.0f, 1.0f) };

		if (TransitionState::FadeOut == transitionState)
		{
			fadeAlpha = normalized;
			if (normalized >= 1.0f)
			{
				SwapToPendingScene();
			}
			return;
		}

		fadeAlpha = 1.0f - normalized;
		if (normalized >= 1.0f)
		{
			transitionState = TransitionState::Idle;
			transitionTimer = 0.0f;
			fadeAlpha = 0.0f;
		}
	}

	void RenderFadeOverlay()
	{
		if (fadeAlpha <= 0.0f ||
			nullptr == fadeMesh ||
			nullptr == fadeMaterial ||
			nullptr == fadeShader)
		{
			return;
		}

		Renderer& renderer{ Application::GetRenderer() };
		renderer.ResetViewport();

		DrawCall drawCall{};
		drawCall.pipelineState = fadeShader->GetPipelineState();
		drawCall.graphicsRootSignature = fadeShader->GetGraphicsRootSignature();
		drawCall.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		drawCall.vertexBufferView = fadeMesh->GetVertexBufferView();
		drawCall.indexBufferView = fadeMesh->GetIndexBufferView();
		drawCall.hasIndexBuffer = fadeMesh->HasIndexBuffer();
		drawCall.indexed = fadeMesh->HasIndexBuffer();
		drawCall.materialDescriptorTable = fadeMaterial->GetDescriptorTable();
		drawCall.materialColor = ColorRGBA(ColorRGB::GetBlack(), fadeAlpha);
		drawCall.vertexCount = fadeMesh->GetVertexCount();
		drawCall.startVertexLocation = 0;
		drawCall.indexCount = fadeMesh->GetIndexCount();
		drawCall.startIndexLocation = 0;
		drawCall.baseVertexLocation = 0;
		drawCall.instanceCount = 1;
		drawCall.startInstanceLocation = 0;
		drawCall.pipelineId = fadeShader->GetPipelineId();
		drawCall.materialId = fadeMaterial->GetId();
		drawCall.meshId = fadeMesh->GetId();
		drawCall.worldTransform = GetFullscreenClipMatrix();
		drawCall.sortKey = BuildSortKey(drawCall.pipelineId, drawCall.materialId, drawCall.meshId);

		renderer.SubmitDrawCall(drawCall);
		renderer.Flush();
	}
}

bool SceneManager::Initialize(ID3D12Device* device_) noexcept
{
	currentScene = nullptr;
	pendingSceneName.clear();
	transitionState = TransitionState::Idle;
	transitionDuration = 0.35f;
	transitionTimer = 0.0f;
	fadeAlpha = 0.0f;

	if (nullptr == device_)
	{
		return false;
	}

	const Mesh::Vertex vertices[]
	{
		{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
	};
	const std::uint32_t indices[]{ 0u, 1u, 2u, 0u, 2u, 3u };

	fadeMesh = std::make_unique<Mesh>();
	if (!fadeMesh->BuildFromRaw(device_, vertices, indices))
	{
		return false;
	}
	fadeMesh->SetId(0xFFFF0001ull);

	fadeShader = std::make_unique<Shader>();
	if (!fadeShader->LoadFromFile(device_, L"Resources/Shaders/UIObject.hlsl"))
	{
		return false;
	}
	fadeShader->SetPipelineId(0xFFFFu);

	fadeMaterial = std::make_unique<Material>();
	fadeMaterial->SetId(0xFFFF0002ull);
	fadeMaterial->SetShader(fadeShader.get());
	fadeMaterial->SetColor(ColorRGBA(ColorRGB::GetBlack(), 0.0f));

	return true;
}

void SceneManager::Release() noexcept
{
	if (nullptr != currentScene)
	{
		currentScene->Unload();
		currentScene = nullptr;
	}

	scenes.clear();
	pendingSceneName.clear();
	fadeMaterial.reset();
	fadeShader.reset();
	fadeMesh.reset();
	transitionState = TransitionState::Idle;
	transitionTimer = 0.0f;
	fadeAlpha = 0.0f;
}

void SceneManager::Update()
{
	UpdateTransition();

	if (nullptr == currentScene)
	{
		return;
	}

	currentScene->Update();
}

void SceneManager::Render()
{
	if (nullptr != currentScene)
	{
		currentScene->Render();
	}

	RenderFadeOverlay();
}

void SceneManager::AddScene(const std::wstring& name_, std::unique_ptr<Scene> scene_)
{
	if (scenes.contains(name_) || nullptr == scene_)
	{
		return;
	}

	scenes.emplace(name_, std::move(scene_));
}

void SceneManager::RemoveScene(const std::wstring& name_)
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	if (currentScene == iterator->second.get())
	{
		currentScene->Unload();
		currentScene = nullptr;
	}

	scenes.erase(iterator);
}

void SceneManager::LoadScene(std::size_t index_) noexcept
{
	if (index_ >= scenes.size())
	{
		return;
	}

	auto iterator{ scenes.begin() };
	std::advance(iterator, static_cast<std::ptrdiff_t>(index_));
	LoadScene(iterator->first);
}

void SceneManager::LoadScene(const std::wstring& name_) noexcept
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	if (nullptr == currentScene)
	{
		LoadSceneImmediately(iterator->second.get());
		return;
	}

	if (currentScene == iterator->second.get())
	{
		return;
	}

	StartTransition(name_);
}

void SceneManager::UnloadScene(std::size_t index_) noexcept
{
	if (index_ >= scenes.size())
	{
		return;
	}

	auto iterator{ scenes.begin() };
	std::advance(iterator, static_cast<std::ptrdiff_t>(index_));
	UnloadScene(iterator->first);
}

void SceneManager::UnloadScene(const std::wstring& name_) noexcept
{
	auto iterator{ scenes.find(name_) };
	if (scenes.end() == iterator)
	{
		return;
	}

	iterator->second->Unload();
	if (currentScene == iterator->second.get())
	{
		currentScene = nullptr;
	}
}

Scene* SceneManager::GetCurrentScene() noexcept
{
	return currentScene;
}

void SceneManager::SetTransitionDuration(float duration_) noexcept
{
	transitionDuration = std::max(0.01f, duration_);
}
