#include "Precompiled.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	void Transform::OnDestroy()
	{
		if (parent)
		{
			std::erase(parent->children, this);
			parent = nullptr;
		}

		for (Transform* child : children)
		{
			child->parent = nullptr;
			child->UpdateMatrices();
		}

		children.clear();
		UpdateMatrices();
	}

	const Vector3D<f32>& Transform::GetLocalPosition() const noexcept
	{
		return localPosition;
	}

	void Transform::SetLocalPosition(const Vector3D<f32>& position) noexcept
	{
		if (!position.IsFinite())
		{
			std::abort();
		}
		localPosition = position;
		UpdateMatrices();
	}

	const Quaternion<f32>& Transform::GetLocalRotation() const noexcept
	{
		return localRotation;
	}

	void Transform::SetLocalRotation(const Quaternion<f32>& rotation) noexcept
	{
		const f32 lengthSquared = rotation.GetSqrMagnitude();
		if (!rotation.IsFinite() || !std::isfinite(lengthSquared) || lengthSquared <= 0.0f)
		{
			std::abort();
		}
		localRotation = rotation.GetNormalized();
		UpdateMatrices();
	}

	const Vector3D<f32>& Transform::GetLocalScale() const noexcept
	{
		return localScale;
	}

	void Transform::SetLocalScale(const Vector3D<f32>& scale) noexcept
	{
		if (!scale.IsFinite())
		{
			std::abort();
		}
		localScale = scale;
		UpdateMatrices();
	}

	Matrix4x4<f32> Transform::GetLocalMatrix() const noexcept
	{
		return Matrix4x4<f32>::TRS(localPosition, localRotation, localScale);
	}

	void Transform::SetLocalMatrix(const Matrix4x4<f32>& matrix) noexcept
	{
		constexpr f32 TOLERANCE = 0.0001f;
		for (usize index = 0; index < 16; ++index)
		{
			if (!std::isfinite(matrix[index]))
			{
				std::abort();
			}
		}
		if (std::abs(matrix[3]) > TOLERANCE || std::abs(matrix[7]) > TOLERANCE ||
			std::abs(matrix[11]) > TOLERANCE || std::abs(matrix[15] - 1.0f) > TOLERANCE)
		{
			std::abort();
		}
		Vector3D<f32> position{};
		Quaternion<f32> rotation{};
		Vector3D<f32> scale{};
		if (!matrix.TryDecompose(position, rotation, scale))
		{
			std::abort();
		}
		const Matrix4x4<f32> recomposed = Matrix4x4<f32>::TRS(position, rotation, scale);
		for (usize index = 0; index < 16; ++index)
		{
			if (!std::isfinite(recomposed[index]) ||
				std::abs(recomposed[index] - matrix[index]) > TOLERANCE * std::max(1.0f, std::abs(matrix[index])))
			{
				std::abort();
			}
		}
		localPosition = position;
		localRotation = rotation;
		localScale = scale;
		UpdateMatrices();
	}

	Vector3D<f32> Transform::GetWorldPosition() const noexcept
	{
		return cachedWorldMatrix.GetWorldPosition();
	}

	void Transform::SetWorldPosition(const Vector3D<f32>& position) noexcept
	{
		if (parent)
		{
			Matrix4x4<f32> inverse{};
			if (!parent->GetWorldMatrix().TryGetInverse(inverse))
			{
				std::abort();
			}
			SetLocalPosition(inverse.MultiplyPoint3x4(position));
		}
		else
		{
			SetLocalPosition(position);
		}
	}

	Quaternion<f32> Transform::GetWorldRotation() const noexcept
	{
		return parent ? (localRotation * parent->GetWorldRotation()).GetNormalized() : localRotation;
	}

	void Transform::SetWorldRotation(const Quaternion<f32>& rotation) noexcept
	{
		if (parent)
		{
			SetLocalRotation(rotation * Quaternion<f32>::Inverse(parent->GetWorldRotation()));
		}
		else
		{
			SetLocalRotation(rotation);
		}
	}

	Vector3D<f32> Transform::GetWorldScale() const noexcept
	{
		if (!parent)
		{
			return localScale;
		}
		const Vector3D<f32> parentScale = parent->GetWorldScale();
		return Vector3D<f32>(localScale.GetX() * parentScale.GetX(), localScale.GetY() * parentScale.GetY(),
			localScale.GetZ() * parentScale.GetZ());
	}

	void Transform::SetWorldScale(const Vector3D<f32>& scale) noexcept
	{
		if (parent)
		{
			const Vector3D<f32> parentScale = parent->GetWorldScale();
			if (!parentScale.IsFinite() || parentScale.GetX() == 0.0f || parentScale.GetY() == 0.0f || parentScale.GetZ() == 0.0f)
			{
				std::abort();
			}
			SetLocalScale(Vector3D<f32>(scale.GetX() / parentScale.GetX(), scale.GetY() / parentScale.GetY(),
				scale.GetZ() / parentScale.GetZ()));
		}
		else
		{
			SetLocalScale(scale);
		}
	}

	const Matrix4x4<f32>& Transform::GetWorldMatrix() const noexcept
	{
		return cachedWorldMatrix;
	}

	void Transform::SetWorldMatrix(const Matrix4x4<f32>& matrix) noexcept
	{
		if (parent)
		{
			Matrix4x4<f32> inverse{};
			if (!parent->GetWorldMatrix().TryGetInverse(inverse))
			{
				std::abort();
			}
			SetLocalMatrix(matrix * inverse);
		}
		else
		{
			SetLocalMatrix(matrix);
		}
	}

	Transform* Transform::GetParent() noexcept
	{
		return parent;
	}

	const Transform* Transform::GetParent() const noexcept
	{
		return parent;
	}

	std::span<Transform* const> Transform::GetChildren() noexcept
	{
		return children;
	}

	std::span<const Transform* const> Transform::GetChildren() const noexcept
	{
		return children;
	}

	void Transform::SetParent(Transform* nextParent) noexcept
	{
		if (parent == nextParent)
		{
			return;
		}
		for (const Transform* ancestor = nextParent; ancestor; ancestor = ancestor->parent)
		{
			if (ancestor == this)
			{
				std::abort();
			}
		}
		if (nextParent)
		{
			nextParent->children.push_back(this);
		}
		if (parent)
		{
			std::erase(parent->children, this);
		}
		parent = nextParent;
		UpdateMatrices();
	}

	void Transform::UpdateMatrices() noexcept
	{
		cachedWorldMatrix = parent ? GetLocalMatrix() * parent->GetWorldMatrix() : GetLocalMatrix();
		for (Transform* child : children)
		{
			child->UpdateMatrices();
		}
	}
}
