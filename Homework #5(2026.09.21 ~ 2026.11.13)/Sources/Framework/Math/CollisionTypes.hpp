#pragma once

#include "Quaternion.hpp"
#include "Vector3D.hpp"
#include "Vector4D.hpp"

#include "../Core/Common.hpp"

namespace TUK::Framework
{
	enum class ContainmentType : u8
	{
		DISJOINT = DirectX::ContainmentType::DISJOINT,
		INTERSECTS = DirectX::ContainmentType::INTERSECTS,
		CONTAINS = DirectX::ContainmentType::CONTAINS
	};

	enum class PlaneIntersectionType : u8
	{
		FRONT = DirectX::PlaneIntersectionType::FRONT,
		INTERSECTING = DirectX::PlaneIntersectionType::INTERSECTING,
		BACK = DirectX::PlaneIntersectionType::BACK
	};

	namespace Detail
	{
		[[nodiscard]] DirectX::XMVECTOR NormalizePlane(const Vector4D<f32>& plane) noexcept;
		[[nodiscard]] DirectX::XMVECTOR NormalizeDirection(const Vector3D<f32>& direction) noexcept;
		[[nodiscard]] Quaternion<f32> NormalizeRotation(const Quaternion<f32>& rotation) noexcept;
		void AssertNonDegenerateTriangle(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) noexcept;
		[[nodiscard]] const DirectX::XMFLOAT3* AsXMFloat3Pointer(std::span<const Vector3D<f32>> points) noexcept;

		template <class TDirectXBox>
		[[nodiscard]] std::array<Vector3D<f32>, 8> ExtractCorners(const TDirectXBox& box) noexcept;

		template <class TDirectXBox>
		void ExtractCorners(const TDirectXBox& box, std::span<Vector3D<f32>, 8> corners) noexcept;

		inline DirectX::XMVECTOR NormalizePlane(const Vector4D<f32>& plane) noexcept
		{
			const Vector3D<f32> normal(plane.GetX(), plane.GetY(), plane.GetZ());
			ASSERT_FMT(plane.IsFinite(), "plane의 성분이 유한한 값이 아닙니다.");
			ASSERT_FMT(normal.GetSqrMagnitude() > std::numeric_limits<f32>::epsilon(), "plane의 법선 벡터가 0입니다.");

			return DirectX::XMPlaneNormalize(Vector4D<f32>::Load(plane));
		}

		inline DirectX::XMVECTOR NormalizeDirection(const Vector3D<f32>& direction) noexcept
		{
			ASSERT_FMT(direction.IsFinite(), "direction의 성분이 유한한 값이 아닙니다.");
			ASSERT_FMT(direction.GetSqrMagnitude() > std::numeric_limits<f32>::epsilon(), "direction이 0 벡터입니다.");

			return Vector3D<f32>::Load(Vector3D<f32>::Normalize(direction));
		}

		inline Quaternion<f32> NormalizeRotation(const Quaternion<f32>& rotation) noexcept
		{
			ASSERT_FMT(rotation.IsFinite(), "rotation의 성분이 유한한 값이 아닙니다.");
			ASSERT_FMT(rotation.GetSqrMagnitude() > std::numeric_limits<f32>::epsilon(), "rotation이 0 쿼터니언입니다.");

			return Quaternion<f32>::Normalize(rotation);
		}

		inline void AssertNonDegenerateTriangle(const Vector3D<f32>& v0, const Vector3D<f32>& v1, const Vector3D<f32>& v2) noexcept
		{
			const Vector3D<f32> normal = Vector3D<f32>::Cross(v1 - v0, v2 - v0);
			ASSERT_FMT(normal.GetSqrMagnitude() > 0.0f, "삼각형이 퇴화(degenerate) 상태입니다.");
		}

		inline const DirectX::XMFLOAT3* AsXMFloat3Pointer(std::span<const Vector3D<f32>> points) noexcept
		{
			static_assert(sizeof(Vector3D<f32>) == sizeof(DirectX::XMFLOAT3));
			static_assert(alignof(Vector3D<f32>) == alignof(DirectX::XMFLOAT3));
			static_assert(std::is_standard_layout_v<Vector3D<f32>>);

			ASSERT_FMT(!points.empty(), "points가 비어 있습니다.");

			return reinterpret_cast<const DirectX::XMFLOAT3*>(points.data());
		}

		template <class TDirectXBox>
		std::array<Vector3D<f32>, 8> ExtractCorners(const TDirectXBox& box) noexcept
		{
			std::array<Vector3D<f32>, 8> corners{};
			ExtractCorners(box, corners);
			return corners;
		}

		template <class TDirectXBox>
		void ExtractCorners(const TDirectXBox& box, std::span<Vector3D<f32>, 8> corners) noexcept
		{
			std::array<DirectX::XMFLOAT3, 8> rawCorners{};
			box.GetCorners(rawCorners.data());

			for (usize index = 0; index < rawCorners.size(); ++index)
			{
				corners[index] = Vector3D<f32>(rawCorners[index]);
			}
		}
	}
}
