#pragma once

#include <limits>

#include "../Math/BoundingBox.hpp"
#include "../Math/BoundingOrientedBox.hpp"
#include "../Math/BoundingSphere.hpp"

#ifdef _DEBUG
#include "../Math/ColorRGBA.hpp"
#endif

#include "Component.hpp"

namespace TUK::Framework
{
#ifdef _DEBUG
	class MaterialAsset;
#endif

	class Collider : public Component
	{
	public:
		using Component::Component;
		~Collider() noexcept override = default;

#ifdef _DEBUG
		void OnAwake() override;

		[[nodiscard]] const MaterialAsset* GetDebugMaterial() const noexcept;
		void SetDebugMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetDebugColor() const noexcept;
		void SetDebugColor(const ColorRGBA<f32>& value) noexcept;
#endif

		[[nodiscard]] bool IsQueryEnabled() const noexcept;
		[[nodiscard]] virtual bool IsValid() const noexcept = 0;

		[[nodiscard]] virtual BoundingBox GetWorldBounds() const noexcept = 0;

		[[nodiscard]] bool Intersects(const Collider* other) const noexcept;
		[[nodiscard]] virtual bool Intersects(const BoundingSphere& sphere) const noexcept = 0;
		[[nodiscard]] virtual bool Intersects(const BoundingOrientedBox& box) const noexcept = 0;

		[[nodiscard]] virtual bool Raycast(
			const Vector3D<f32>& origin,
			const Vector3D<f32>& direction,
			f32& distance,
			f32 maxDistance = std::numeric_limits<f32>::max()) const noexcept = 0;

	protected:
#ifdef _DEBUG
		void LoadDebugMaterial(const std::filesystem::path& path);
		[[nodiscard]] bool PrepareDebugRender(GPUCommandBuffer& commandBuffer, const Matrix4x4<f32>& world) const;
		void DrawDebugLines(GPUCommandBuffer& commandBuffer, std::span<const Vector3D<f32>> vertices) const;

#endif
		[[nodiscard]] Matrix4x4<f32> GetWorldMatrix() const noexcept;
		[[nodiscard]] bool HasValidTransform() const noexcept;
		[[nodiscard]] static bool TryNormalizeRay(
			const Vector3D<f32>& origin,
			const Vector3D<f32>& direction,
			f32 maxDistance,
			Vector3D<f32>& normalizedDirection) noexcept;

#ifdef _DEBUG
	private:
		const MaterialAsset* debugMaterial = nullptr;
		ColorRGBA<f32> debugColor{ 0.0f, 1.0f, 0.0f, 1.0f };
#endif
	};

#ifdef _DEBUG
	inline const MaterialAsset* Collider::GetDebugMaterial() const noexcept
	{
		return debugMaterial;
	}

	inline void Collider::SetDebugMaterial(const MaterialAsset* value) noexcept
	{
		debugMaterial = value;
	}

	inline const ColorRGBA<f32>& Collider::GetDebugColor() const noexcept
	{
		return debugColor;
	}

	inline void Collider::SetDebugColor(const ColorRGBA<f32>& value) noexcept
	{
		debugColor = value;
	}
#endif
}
