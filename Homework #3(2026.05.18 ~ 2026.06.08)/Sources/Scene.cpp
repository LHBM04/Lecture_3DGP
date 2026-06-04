#include "Precompiled.h"
#include "Scene.h"

GameObject* Scene::FindObjectWithName(std::wstring_view name_)
{
	auto iter{ std::ranges::find_if(gameObjects, [name_](const auto& gameObject)
		{
			return gameObject->GetName() == name_;
		}) };
	if (iter != gameObjects.end())
	{
		return iter->get();
	}

	return nullptr;
}

auto Scene::FindObjectsWithName(std::wstring_view name_)
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

GameObject* Scene::FindObjectWithTag(std::wstring_view tag_)
{
	auto iter{ std::ranges::find_if(gameObjects, [tag_](const auto& gameObject)
		{
			return gameObject->GetTag() == tag_;
		}) };
	if (iter != gameObjects.end())
	{
		return iter->get();
	}
	return nullptr;
}

auto Scene::FindObjectsWithTag(std::wstring_view tag_)
{
	return gameObjects
		| std::views::filter([tag_](const auto& gameObject)
			{
				return gameObject->GetTag() == tag_;
			})
		| std::views::transform([](const auto& gameObject)
			{
				return gameObject.get();
			});
}
