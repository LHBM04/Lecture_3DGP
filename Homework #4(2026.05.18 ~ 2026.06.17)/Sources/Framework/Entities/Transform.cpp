#include "Precompiled.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
	Transform::Transform() noexcept
		: localPosition()
		, localRotation(Quaternion<float>::GetIdentity())
		, localScale(Vector3D<float>::GetOne())
		, parent(nullptr)
		, children()
	{
	}

	Transform::~Transform() noexcept
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

	const Vector3D<float>& Transform::GetLocalPosition() const noexcept
	{
		return localPosition;
	}

	void Transform::SetLocalPosition(const Vector3D<float>& position)
	{
		if (!position.IsFinite()) throw std::invalid_argument("위치에는 유한한 값이 필요합니다.");
		localPosition = position;
		UpdateMatrices();
	}

	const Quaternion<float>& Transform::GetLocalRotation() const noexcept
	{
		return localRotation;
	}

	void Transform::SetLocalRotation(const Quaternion<float>& rotation)
	{
		const float lengthSquared = rotation.GetSqrMagnitude();
		if (!rotation.IsFinite() || !std::isfinite(lengthSquared) || lengthSquared <= 0.0f)
		{
			throw std::invalid_argument("회전에는 정규화 가능한 유한한 쿼터니언이 필요합니다.");
		}
		localRotation = rotation.GetNormalized();
		UpdateMatrices();
	}

	const Vector3D<float>& Transform::GetLocalScale() const noexcept
	{
		return localScale;
	}

	void Transform::SetLocalScale(const Vector3D<float>& scale)
	{
		if (!scale.IsFinite()) throw std::invalid_argument("스케일에는 유한한 값이 필요합니다.");
		localScale = scale;
		UpdateMatrices();
	}

	Matrix4x4<float> Transform::GetLocalMatrix() const noexcept
	{
		return Matrix4x4<float>::TRS(localPosition, localRotation, localScale);
	}

	void Transform::SetLocalMatrix(const Matrix4x4<float>& matrix)
	{
		constexpr float tolerance = 0.0001f;
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (!std::isfinite(matrix[index])) throw std::invalid_argument("행렬에 유한하지 않은 값이 있습니다.");
		}
		if (std::abs(matrix[3]) > tolerance || std::abs(matrix[7]) > tolerance
			|| std::abs(matrix[11]) > tolerance || std::abs(matrix[15] - 1.0f) > tolerance)
		{
			throw std::invalid_argument("Transform에는 아핀 행렬만 지정할 수 있습니다.");
		}
		Vector3D<float> position{};
		Quaternion<float> rotation{};
		Vector3D<float> scale{};
		if (!matrix.TryDecompose(position, rotation, scale))
		{
			throw std::invalid_argument("행렬을 위치, 회전, 스케일로 분해할 수 없습니다.");
		}
		const Matrix4x4<float> recomposed = Matrix4x4<float>::TRS(position, rotation, scale);
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (!std::isfinite(recomposed[index])
				|| std::abs(recomposed[index] - matrix[index]) > tolerance * std::max(1.0f, std::abs(matrix[index])))
			{
				throw std::invalid_argument("전단을 포함한 행렬은 로컬 TRS로 표현할 수 없습니다.");
			}
		}
		localPosition = position;
		localRotation = rotation;
		localScale = scale;
		UpdateMatrices();
	}

	Vector3D<float> Transform::GetWorldPosition() const noexcept
	{
		return cachedWorldMatrix.GetWorldPosition();
	}

	void Transform::SetWorldPosition(const Vector3D<float>& position)
	{
		if (parent)
		{
			Matrix4x4<float> inverse{};
			if (!parent->GetWorldMatrix().TryGetInverse(inverse))
			{
				throw std::invalid_argument("부모 월드 행렬의 역행렬을 구할 수 없습니다.");
			}
			SetLocalPosition(inverse.MultiplyPoint3x4(position));
		}
		else SetLocalPosition(position);
	}

	Quaternion<float> Transform::GetWorldRotation() const noexcept
	{
		// 로컬 회전을 적용한 뒤 부모 회전을 적용한다.
		return parent ? (localRotation * parent->GetWorldRotation()).GetNormalized() : localRotation;
	}

	void Transform::SetWorldRotation(const Quaternion<float>& rotation)
	{
		if (parent) SetLocalRotation(rotation * Quaternion<float>::Inverse(parent->GetWorldRotation()));
		else SetLocalRotation(rotation);
	}

	Vector3D<float> Transform::GetWorldScale() const noexcept
	{
		if (!parent) return localScale;
		const Vector3D<float> parentScale = parent->GetWorldScale();
		return Vector3D<float>(localScale.GetX() * parentScale.GetX(), localScale.GetY() * parentScale.GetY(),
			localScale.GetZ() * parentScale.GetZ());
	}

	void Transform::SetWorldScale(const Vector3D<float>& scale)
	{
		if (parent)
		{
			const Vector3D<float> parentScale = parent->GetWorldScale();
			if (!parentScale.IsFinite() || parentScale.GetX() == 0.0f || parentScale.GetY() == 0.0f || parentScale.GetZ() == 0.0f)
			{
				throw std::invalid_argument("부모 스케일이 유한하지 않거나 0이므로 월드 스케일을 설정할 수 없습니다.");
			}
			SetLocalScale(Vector3D<float>(scale.GetX() / parentScale.GetX(), scale.GetY() / parentScale.GetY(),
				scale.GetZ() / parentScale.GetZ()));
		}
		else SetLocalScale(scale);
	}

	const Matrix4x4<float>& Transform::GetWorldMatrix() const noexcept
	{
		return cachedWorldMatrix;
	}

	void Transform::SetWorldMatrix(const Matrix4x4<float>& matrix)
	{
		if (parent)
		{
			Matrix4x4<float> inverse{};
			if (!parent->GetWorldMatrix().TryGetInverse(inverse))
			{
				throw std::invalid_argument("부모 월드 행렬의 역행렬을 구할 수 없습니다.");
			}
			SetLocalMatrix(matrix * inverse);
		}
		else SetLocalMatrix(matrix);
	}

	Transform* Transform::GetParent() noexcept { return parent; }
	const Transform* Transform::GetParent() const noexcept { return parent; }
	std::span<Transform* const> Transform::GetChildren() noexcept { return children; }
	std::span<const Transform* const> Transform::GetChildren() const noexcept { return children; }

	void Transform::SetParent(Transform* nextParent)
	{
		if (parent == nextParent) return;
		for (const Transform* ancestor = nextParent; ancestor; ancestor = ancestor->parent)
		{
			if (ancestor == this) throw std::invalid_argument("자기 자신이나 자손을 부모로 지정할 수 없습니다.");
		}
		if (nextParent) nextParent->children.push_back(this);
		if (parent) std::erase(parent->children, this);
		parent = nextParent;
		UpdateMatrices();
	}

	void Transform::UpdateMatrices() noexcept
	{
		cachedWorldMatrix = parent ? GetLocalMatrix() * parent->GetWorldMatrix() : GetLocalMatrix();
		for (Transform* child : children) child->UpdateMatrices();
	}
}
