#pragma once

#include <span>
#include <vector>

#include "../Core/Common.hpp"

#include "../Math/Matrix4x4.hpp"
#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "Component.hpp"

namespace TUK::Framework
{
	class GameObject;

	class Transform : public Component
	{
	public:
		using Component::Component;

		[[nodiscard]] const Vector3D<f32>& GetLocalPosition() const noexcept;
		void SetLocalPosition(const Vector3D<f32>& position) noexcept;

		[[nodiscard]] const Quaternion<f32>& GetLocalRotation() const noexcept;
		void SetLocalRotation(const Quaternion<f32>& rotation) noexcept;

		[[nodiscard]] const Vector3D<f32>& GetLocalScale() const noexcept;
		void SetLocalScale(const Vector3D<f32>& scale) noexcept;

		[[nodiscard]] Vector3D<f32> GetWorldPosition() const noexcept;
		void SetWorldPosition(const Vector3D<f32>& position) noexcept;

		[[nodiscard]] Quaternion<f32> GetWorldRotation() const noexcept;
		void SetWorldRotation(const Quaternion<f32>& rotation) noexcept;

		[[nodiscard]] Vector3D<f32> GetWorldScale() const noexcept;
		void SetWorldScale(const Vector3D<f32>& scale) noexcept;

		[[nodiscard]] Matrix4x4<f32> GetLocalMatrix() const noexcept;
		void SetLocalMatrix(const Matrix4x4<f32>& matrix) noexcept;

		[[nodiscard]] const Matrix4x4<f32>& GetWorldMatrix() const noexcept;
		void SetWorldMatrix(const Matrix4x4<f32>& matrix) noexcept;

		[[nodiscard]] Transform* GetParent() noexcept;
		[[nodiscard]] const Transform* GetParent() const noexcept;
		void SetParent(Transform* parent) noexcept;

		[[nodiscard]] std::span<Transform* const> GetChildren() noexcept;
		[[nodiscard]] std::span<const Transform* const> GetChildren() const noexcept;

		void OnDestroy() override;

	private:
		void UpdateMatrices() noexcept;

		Vector3D<f32> localPosition{ 0.0f };
		Quaternion<f32> localRotation{ 0.0f };
		Vector3D<f32> localScale{ 1.0f };

		Transform* parent = nullptr;
		std::vector<Transform*> children{};

		Matrix4x4<f32> cachedWorldMatrix{ Matrix4x4<f32>::GetIdentity() };
	};
}
