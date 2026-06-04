#include "Precompiled.h"
#include "Scene.h"

auto& Scene::FindObjectsWithName(std::wstring_view name_)
{
	return gameObjects
		| std::views::filter([name_](const auto& gameObject)
			{
				return gameObject->GetName() == name_;
			})
		| std::views::transform([](const auto& gameObject)
			{
				return gameObject.get();
			});
}
