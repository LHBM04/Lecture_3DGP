#pragma once

#include "Framework/Math.hpp"
#include "Framework/Objects.hpp"

namespace TUK::Game
{
	using namespace Framework;

	[[nodiscard]] inline const Terrain* FindValidTerrain(const Scene* scene) noexcept
	{
		if (!scene)
		{
			return nullptr;
		}

		const GameObject* terrainObject = scene->FindGameObjectByTag("Terrain");
		if (!terrainObject)
		{
			return nullptr;
		}

		const Terrain* terrain = terrainObject->GetComponent<Terrain>();
		return (terrain && terrain->IsValid()) ? terrain : nullptr;
	}

	[[nodiscard]] inline Quaternion<f32> ExtractHorizontalYawRotation(const Quaternion<f32>& rotation) noexcept
	{
		Vector3D<f32> horizontalForward = rotation * Vector3D<f32>::GetForward();
		horizontalForward.SetY(0.0f);
		if (horizontalForward.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			return Quaternion<f32>::GetIdentity();
		}

		horizontalForward.Normalize();
		return Quaternion<f32>::LookRotation(horizontalForward, Vector3D<f32>::GetUp());
	}

	[[nodiscard]] inline f32 ComputeTiltBlendFactor(f32 alignSpeed, f32 deltaTime) noexcept
	{
		if (alignSpeed <= 0.0f)
		{
			return 1.0f;
		}

		return Mathf::Clamp(alignSpeed * deltaTime, 0.0f, 1.0f);
	}

	[[nodiscard]] inline Quaternion<f32> ComputeSlopeAlignedRotation(
		const Vector3D<f32>& forwardDirection,
		const Vector3D<f32>& surfaceNormal) noexcept
	{
		Vector3D<f32> up = surfaceNormal;
		if (up.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			up = Vector3D<f32>::GetUp();
		}
		else
		{
			up.Normalize();
		}

		Vector3D<f32> alignedForward = Vector3D<f32>::ProjectOnPlane(forwardDirection, up);
		if (alignedForward.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			alignedForward = forwardDirection;
		}

		if (alignedForward.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			alignedForward = Vector3D<f32>::GetForward();
		}
		else
		{
			alignedForward.Normalize();
		}

		return Quaternion<f32>::LookRotation(alignedForward, up);
	}

	[[nodiscard]] inline Quaternion<f32> AlignTankToTerrain(
		const Scene* scene,
		Vector3D<f32>& worldPosition,
		Vector3D<f32>& smoothedUp,
		const Vector3D<f32>& forwardDirection,
		f32 blendFactor) noexcept
	{
		Vector3D<f32> targetUp = Vector3D<f32>::GetUp();
		if (const Terrain* terrain = FindValidTerrain(scene))
		{
			worldPosition.SetY(terrain->SampleHeightAtWorldPosition(worldPosition));
			targetUp = terrain->SampleNormalAtWorldPosition(worldPosition);
		}

		const f32 clampedBlend = Mathf::Clamp(blendFactor, 0.0f, 1.0f);
		if (smoothedUp.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			smoothedUp = targetUp;
		}
		else
		{
			smoothedUp = Vector3D<f32>::Slerp(smoothedUp, targetUp, clampedBlend);
		}

		if (smoothedUp.GetSqrMagnitude() <= Mathf::Epsilon)
		{
			smoothedUp = Vector3D<f32>::GetUp();
		}
		else
		{
			smoothedUp.Normalize();
		}

		return ComputeSlopeAlignedRotation(forwardDirection, smoothedUp);
	}
}
