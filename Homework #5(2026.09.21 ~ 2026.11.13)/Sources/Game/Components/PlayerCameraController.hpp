#pragma once

#include "Framework/Objects.hpp"
#include "Framework/Resources.hpp"

namespace TUK::Game
{
	using namespace Framework;

	class PlayerCameraController final : public Component
	{
	public:
		using Component::Component;

		void OnStart() override;
		void OnLateUpdate() override;
		void OnPostRender(GPUCommandBuffer& commandBuffer) override;

		[[nodiscard]] const Transform* GetTarget() const noexcept;
		void SetTarget(const Transform* value) noexcept;

		[[nodiscard]] f32 GetFollowDistance() const noexcept;
		void SetFollowDistance(f32 value) noexcept;

		[[nodiscard]] f32 GetFollowHeight() const noexcept;
		void SetFollowHeight(f32 value) noexcept;

		[[nodiscard]] f32 GetSmoothSpeed() const noexcept;
		void SetSmoothSpeed(f32 value) noexcept;

		[[nodiscard]] bool IsFirstPerson() const noexcept;
		void SetFirstPerson(bool value) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetFirstPersonOffset() const noexcept;
		void SetFirstPersonOffset(const Vector3D<f32>& value) noexcept;

	private:
		static constexpr f32 DEFAULT_FOLLOW_DISTANCE = 10.0f;
		static constexpr f32 DEFAULT_FOLLOW_HEIGHT = 3.0f;
		static constexpr f32 DEFAULT_SMOOTH_SPEED = 10.0f;

		static constexpr f32 DEFAULT_FIRST_PERSON_HEIGHT = 1.5f;
		static constexpr f32 DEFAULT_FIRST_PERSON_FORWARD = 0.5f;

		static constexpr f32 CROSSHAIR_SIZE = 32.0f;

		const Transform* target = nullptr;
		f32 followDistance = DEFAULT_FOLLOW_DISTANCE;
		f32 followHeight = DEFAULT_FOLLOW_HEIGHT;
		f32 smoothSpeed = DEFAULT_SMOOTH_SPEED;
		Vector3D<f32> firstPersonOffset{ 0.0f, DEFAULT_FIRST_PERSON_HEIGHT, DEFAULT_FIRST_PERSON_FORWARD };

		bool isFirstPerson = false;
		bool isFirstFrame = true;

		const TextureAsset* crosshairTexture = nullptr;
		const MaterialAsset* crosshairMaterial = nullptr;
	};

	inline const Transform* PlayerCameraController::GetTarget() const noexcept
	{
		return target;
	}

	inline void PlayerCameraController::SetTarget(const Transform* value) noexcept
	{
		target = value;
		isFirstFrame = true;
	}

	inline f32 PlayerCameraController::GetFollowDistance() const noexcept
	{
		return followDistance;
	}

	inline void PlayerCameraController::SetFollowDistance(f32 value) noexcept
	{
		ASSERT_FMT(value > 0.0f, "카메라 추적 거리는 0보다 커야 합니다.");
		followDistance = value;
	}

	inline f32 PlayerCameraController::GetFollowHeight() const noexcept
	{
		return followHeight;
	}

	inline void PlayerCameraController::SetFollowHeight(f32 value) noexcept
	{
		followHeight = value;
	}

	inline f32 PlayerCameraController::GetSmoothSpeed() const noexcept
	{
		return smoothSpeed;
	}

	inline void PlayerCameraController::SetSmoothSpeed(f32 value) noexcept
	{
		smoothSpeed = value;
	}

	inline bool PlayerCameraController::IsFirstPerson() const noexcept
	{
		return isFirstPerson;
	}

	inline void PlayerCameraController::SetFirstPerson(bool value) noexcept
	{
		isFirstPerson = value;
	}

	inline const Vector3D<f32>& PlayerCameraController::GetFirstPersonOffset() const noexcept
	{
		return firstPersonOffset;
	}

	inline void PlayerCameraController::SetFirstPersonOffset(const Vector3D<f32>& value) noexcept
	{
		firstPersonOffset = value;
	}
}
