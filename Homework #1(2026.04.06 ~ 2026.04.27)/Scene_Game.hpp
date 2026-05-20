#pragma once

#include "Scene.hpp"

class Camera;
class Object;
class TextView;

class Scene_Game : public Scene
{
public:
	virtual ~Scene_Game() override = default;

	[[nodiscard]] static std::size_t GetPlayerScore();
	static void SetPlayerScore(std::size_t score_);
	static void AddPlayerScore(std::size_t score_);

protected:
	virtual void OnLoad() override;
	virtual void OnUpdate() override;
	virtual void OnRender() override;

private:
	void ToggleCameraProjection();
	void ApplyCameraSettings(Camera* camera_) const;

	static std::size_t playerScore;
	TextView* scoreTextView{ nullptr };
	Object* cameraObject{ nullptr };
	bool isOrthographicCameraActive{ false };
};
