#pragma once

#include "Scene.h"
#include "System.h"

struct SceneBuildEntry final
{
	std::wstring name;
	std::unique_ptr<Scene>(*create)();
};

struct SceneOptions final
{
	std::initializer_list<SceneBuildEntry> scenes;
	std::size_t startIndex{ 0 };
};

class SceneSystem : public System<SceneOptions>
{
public:
	~SceneSystem() noexcept override;

	bool Initialize(const SceneOptions& options_) override;
	void Release() override;

	void Update();
	void Render();

	template <std::derived_from<Scene> TScene>
	void AddScene(std::wstring name_);
	
	void LoadScene(std::size_t index_);
	void LoadScene(std::wstring name_);

	void UnloadScene();
	
	Scene* GetCurrentScene() const noexcept;

private:
	std::vector<std::unique_ptr<Scene>> scenes;
	std::unordered_map<std::wstring, std::size_t> forName;

	Scene* currentScene{ nullptr };
	Scene* nextScene{ nullptr };
};

template<std::derived_from<Scene> TScene>
inline void SceneSystem::AddScene(std::wstring name_)
{
	std::unique_ptr<TScene> scene{ std::make_unique<TScene>() };

	forName.emplace(name_, scene.get());
	scenes.emplace_back(std::move(scene));
}
