#pragma once

#include <span>
#include <vector>

#include "Framework/Math/Matrix4x4.hpp"
#include "Framework/Math/Quaternion.hpp"
#include "Framework/Math/Vector3D.hpp"
#include "Framework/Entities/Component.hpp"

namespace TUK::Framework
{
	class Transform : public Component
	{
	public:
		Transform() noexcept;
		~Transform() noexcept override;

		/** 로컬 위치 */
		[[nodiscard]] const Vector3D<float>& GetLocalPosition() const noexcept;
		void SetLocalPosition(const Vector3D<float>& position);

		/** 로컬 회전 */
		[[nodiscard]] const Quaternion<float>& GetLocalRotation() const noexcept;
		void SetLocalRotation(const Quaternion<float>& rotation);

		/** 로컬 크기 */
		[[nodiscard]] const Vector3D<float>& GetLocalScale() const noexcept;
		void SetLocalScale(const Vector3D<float>& scale);

		/** 월드 위치 */
		[[nodiscard]] Vector3D<float> GetWorldPosition() const noexcept;
		void SetWorldPosition(const Vector3D<float>& position);

		/** 월드 회전 */
		[[nodiscard]] Quaternion<float> GetWorldRotation() const noexcept;
		void SetWorldRotation(const Quaternion<float>& rotation);

		/** 월드 크기 */
		[[nodiscard]] Vector3D<float> GetWorldScale() const noexcept;
		void SetWorldScale(const Vector3D<float>& scale);

		/** 로컬 행렬 */
		[[nodiscard]] Matrix4x4<float> GetLocalMatrix() const noexcept;
		void SetLocalMatrix(const Matrix4x4<float>& matrix);

		/** 월드 행렬 */
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
		/** 본인 & 자식 행렬 갱신 */
		void UpdateMatrices() noexcept;

		/** 로컬 위치 */
		Vector3D<float> localPosition;

		/** 로컬 회전 */
		Quaternion<float> localRotation;

		/** 로컬 스케일 */
		Vector3D<float> localScale;

		/** 부모 Transform */
		Transform* parent;
		
		/** 자식 Transform */
		std::vector<Transform*> children;

		/** 월드 행렬 */
		Matrix4x4<float> cachedWorldMatrix{};
	};
}
