#pragma once

#include <random>
#include <string>
#include <string_view>

#include "Component.h"

class GameObject;

class TitleController final : public Component
{
public:
	TitleController() = default;
	~TitleController() override = default;

	void SetTitleLogoObject(GameObject* titleLogoObject_) noexcept;
	void SetTutorialButtonObject(GameObject* tutorialButtonObject_) noexcept;
	void SetLevel1ButtonObject(GameObject* level1ButtonObject_) noexcept;
	void SetLevel2ButtonObject(GameObject* level2ButtonObject_) noexcept;
	void SetLevel3ButtonObject(GameObject* level3ButtonObject_) noexcept;
	void SetStartButtonObject(GameObject* startButtonObject_) noexcept;
	void SetEndButtonObject(GameObject* endButtonObject_) noexcept;

protected:
	void OnStart() override;
	void OnUpdate() override;

private:
	void UpdateTitleLogoRotation();
	void RevealMenuButtons();
	void HideMenuButtons();
	void ExplodeTitleLogo();
	void SelectLevel(std::wstring_view sceneName_, GameObject* buttonObject_);

private:
	GameObject* titleLogoObject{ nullptr };
	GameObject* tutorialButtonObject{ nullptr };
	GameObject* level1ButtonObject{ nullptr };
	GameObject* level2ButtonObject{ nullptr };
	GameObject* level3ButtonObject{ nullptr };
	GameObject* startButtonObject{ nullptr };
	GameObject* endButtonObject{ nullptr };

	bool isMenuVisible{ false };
	std::wstring selectedSceneName{ L"" };

	std::mt19937 randomEngine{ std::random_device{}() };
};
