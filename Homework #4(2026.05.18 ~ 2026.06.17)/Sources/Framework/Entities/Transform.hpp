#pragma once

#include <span>
#include <vector>

#include "../Math/Matrix4x4.hpp"
#include "../Math/Quaternion.hpp"
#include "../Math/Vector3D.hpp"

#include "Component.hpp"

namespace TUK::Framework
{
	class Entity;

	class Transform : public Component
	{
	public:
		Transform(Entity& owner) noexcept;
		~Transform() noexcept override;

		[[nodiscard]] const Vector3D<float>& GetLocalPosition() const noexcept;
		void SetLocalPosition(const Vector3D<float>& position);

		[[nodiscard]] const Quaternion<float>& GetLocalRotation() const noexcept;
		void SetLocalRotation(const Quaternion<float>& rotation);

		[[nodiscard]] const Vector3D<float>& GetLocalScale() const noexcept;
		void SetLocalScale(const Vector3D<float>& scale);

		[[nodiscard]] Vector3D<float> GetWorldPosition() const noexcept;
		void SetWorldPosition(const Vector3D<float>& position);

		[[nodiscard]] Quaternion<float> GetWorldRotation() const noexcept;
		void SetWorldRotation(const Quaternion<float>& rotation);

		[[nodiscard]] Vector3D<float> GetWorldScale() const noexcept;
		void SetWorldScale(const Vector3D<float>& scale);

		[[nodiscard]] Matrix4x4<float> GetLocalMatrix() const noexcept;
		void SetLocalMatrix(const Matrix4x4<float>& matrix);

		[[nodiscard]] const Matrix4x4<float>& GetWorldMatrix() const noexcept;
		void SetWorldMatrix(const Matrix4x4<float>& matrix);

		[[nodiscard]] Transform* GetParent() noexcept;
		[[nodiscard]] const Transform* GetParent() const noexcept;
		void SetParent(Transform* parent);

		[[nodiscard]] std::span<Transform* const> GetChildren() noexcept;
		[[nodiscard]] std::span<const Transform* const> GetChildren() const noexcept;
		
	protected:
		void OnDestroy() override;

	private:
		void UpdateMatrices() noexcept;

		Vector3D<float> localPosition;
		Quaternion<float> localRotation;
		Vector3D<float> localScale;

		Transform* parent;
		std::vector<Transform*> children;

		Matrix4x4<float> cachedWorldMatrix{};
	};
}
