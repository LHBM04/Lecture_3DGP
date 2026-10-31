#include "Precompiled.hpp"
#include "Matrix4x4.hpp"

#include "Quaternion.hpp"

namespace TUK::Framework
{
	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4() noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = static_cast<TScalar>(1);
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(TScalar value) noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = value;
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(const TScalar values[16]) noexcept
		: value{}
	{
		assert(values);
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = values[index];
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(
		TScalar m00, TScalar m01, TScalar m02, TScalar m03,
		TScalar m10, TScalar m11, TScalar m12, TScalar m13,
		TScalar m20, TScalar m21, TScalar m22, TScalar m23,
		TScalar m30, TScalar m31, TScalar m32, TScalar m33) noexcept
		: value{}
	{
		const TScalar components[16]{
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33
		};
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = components[index];
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(const Matrix4x4<TScalar>& other) noexcept
		: value(other.value)
	{
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(Matrix4x4<TScalar>&& other) noexcept
		: value(other.value)
	{
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>::Matrix4x4(DirectX::XMMATRIX matrix) noexcept
		requires std::same_as<TScalar, f32>
		: value{}
	{
		Store(*this, matrix);
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>& Matrix4x4<TScalar>::operator=(const Matrix4x4<TScalar>& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>& Matrix4x4<TScalar>::operator=(Matrix4x4<TScalar>&& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <ScalarType TScalar>
	TScalar Matrix4x4<TScalar>::operator[](size_t index) const noexcept
	{
		assert(index < 16);
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return value[index];
		}
		else
		{
			return value.m[index / 4][index % 4];
		}
	}

	template <ScalarType TScalar>
	TScalar& Matrix4x4<TScalar>::operator[](size_t index) noexcept
	{
		assert(index < 16);
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return value[index];
		}
		else
		{
			return value.m[index / 4][index % 4];
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::operator*(const Matrix4x4<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			return Matrix4x4<TScalar>(DirectX::XMMatrixMultiply(Load(*this), Load(other)));
		}
		else
		{
			Matrix4x4<TScalar> result(TScalar{});
			for (std::size_t row = 0; row < 4; ++row)
			{
				for (std::size_t column = 0; column < 4; ++column)
				{
					for (std::size_t element = 0; element < 4; ++element)
					{
						result[row * 4 + column] += (*this)[row * 4 + element] * other[element * 4 + column];
					}
				}
			}
			return result;
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar>& Matrix4x4<TScalar>::operator*=(const Matrix4x4<TScalar>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::operator==(const Matrix4x4<TScalar>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::operator!=(const Matrix4x4<TScalar>& other) const noexcept
	{
		return !(*this == other);
	}

	template <ScalarType TScalar>
	std::partial_ordering Matrix4x4<TScalar>::operator<=>(const Matrix4x4<TScalar>& other) const noexcept
	{
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (const auto order = (*this)[index] <=> other[index]; order != 0)
			{
				return order;
			}
		}
		return std::partial_ordering::equivalent;
	}

	template <ScalarType TScalar>
	Vector4D<TScalar> Matrix4x4<TScalar>::GetRow(std::size_t index) const noexcept
	{
		assert(index < 4);
		const auto base = index * 4;
		return Vector4D<TScalar>((*this)[base], (*this)[base + 1], (*this)[base + 2], (*this)[base + 3]);
	}

	template <ScalarType TScalar>
	void Matrix4x4<TScalar>::SetRow(std::size_t index, const Vector4D<TScalar>& row) noexcept
	{
		assert(index < 4);
		for (std::size_t column = 0; column < 4; ++column)
		{
			(*this)[index * 4 + column] = row[column];
		}
	}

	template <ScalarType TScalar>
	Vector4D<TScalar> Matrix4x4<TScalar>::GetColumn(std::size_t index) const noexcept
	{
		assert(index < 4);
		return Vector4D<TScalar>((*this)[index], (*this)[index + 4], (*this)[index + 8], (*this)[index + 12]);
	}

	template <ScalarType TScalar>
	void Matrix4x4<TScalar>::SetColumn(std::size_t index, const Vector4D<TScalar>& column) noexcept
	{
		assert(index < 4);
		for (std::size_t row = 0; row < 4; ++row)
		{
			(*this)[row * 4 + index] = column[row];
		}
	}

	template <ScalarType TScalar>
	float Matrix4x4<TScalar>::GetDeterminant() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(Load(*this)));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::GetTranspose() const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			return Matrix4x4<TScalar>(DirectX::XMMatrixTranspose(Load(*this)));
		}
		else
		{
			Matrix4x4<TScalar> result(TScalar{});
			for (std::size_t row = 0; row < 4; ++row)
			{
				for (std::size_t column = 0; column < 4; ++column)
				{
					result[row * 4 + column] = (*this)[column * 4 + row];
				}
			}
			return result;
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::GetInverse() const noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMVECTOR determinant;
		const auto inverse = DirectX::XMMatrixInverse(&determinant, Load(*this));
		const float scalar = DirectX::XMVectorGetX(determinant);
		assert(std::isfinite(scalar) && scalar != 0.0f);
		return Matrix4x4<TScalar>(inverse);
	}

	template <ScalarType TScalar>
	Vector3D<TScalar> Matrix4x4<TScalar>::GetWorldPosition() const noexcept
	{
		return Vector3D<TScalar>((*this)[12], (*this)[13], (*this)[14]);
	}

	template <ScalarType TScalar>
	Quaternion<TScalar> Matrix4x4<TScalar>::GetRotation() const noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector3D<float> scale = GetScale();
		if (scale.GetX() <= std::numeric_limits<float>::epsilon() || scale.GetY() <= std::numeric_limits<float>::epsilon() || scale.GetZ() <= std::numeric_limits<float>::epsilon())
		{
			return Quaternion<TScalar>::GetIdentity();
		}

		DirectX::XMMATRIX rotation = Load(*this);
		rotation.r[0] = DirectX::XMVectorScale(rotation.r[0], 1.0f / scale.GetX());
		rotation.r[1] = DirectX::XMVectorScale(rotation.r[1], 1.0f / scale.GetY());
		rotation.r[2] = DirectX::XMVectorScale(rotation.r[2], 1.0f / scale.GetZ());
		rotation.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		Quaternion<TScalar> result{};
		Quaternion<TScalar>::Store(result, DirectX::XMQuaternionRotationMatrix(rotation));
		return result;
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::GetScale() const noexcept
		requires std::same_as<TScalar, f32>
	{
		const Vector3D<float> xAxis(value._11, value._12, value._13);
		const Vector3D<float> yAxis(value._21, value._22, value._23);
		const Vector3D<float> zAxis(value._31, value._32, value._33);
		return Vector3D<float>(xAxis.GetMagnitude(), yAxis.GetMagnitude(), zAxis.GetMagnitude());
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::GetLossyScale() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return GetScale();
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::GetForward() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<float>(value._31, value._32, value._33).GetNormalized();
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::GetUp() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<float>(value._21, value._22, value._23).GetNormalized();
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::GetRight() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Vector3D<float>(value._11, value._12, value._13).GetNormalized();
	}

	template <ScalarType TScalar>
	void Matrix4x4<TScalar>::SetTRS(const Vector3D<float>& position, const Quaternion<TScalar>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TScalar, f32>
	{
		*this = TRS(position, rotation, scale);
	}

	template <ScalarType TScalar>
	Vector3D<TScalar> Matrix4x4<TScalar>::MultiplyPoint3x4(const Vector3D<TScalar>& point) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			Vector3D<TScalar> result{};
			Vector3D<TScalar>::Store(result, DirectX::XMVector3Transform(Vector3D<TScalar>::Load(point), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TScalar>(
				point.GetX() * (*this)[0] + point.GetY() * (*this)[4] + point.GetZ() * (*this)[8] + (*this)[12],
				point.GetX() * (*this)[1] + point.GetY() * (*this)[5] + point.GetZ() * (*this)[9] + (*this)[13],
				point.GetX() * (*this)[2] + point.GetY() * (*this)[6] + point.GetZ() * (*this)[10] + (*this)[14]);
		}
	}

	template <ScalarType TScalar>
	Vector3D<float> Matrix4x4<TScalar>::MultiplyPoint(const Vector3D<float>& point) const noexcept
		requires std::same_as<TScalar, f32>
	{
		Vector3D<float> result{};
		Vector3D<float>::Store(result, DirectX::XMVector3TransformCoord(Vector3D<float>::Load(point), Load(*this)));
		return result;
	}

	template <ScalarType TScalar>
	Vector3D<TScalar> Matrix4x4<TScalar>::MultiplyVector(const Vector3D<TScalar>& vector) const noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			Vector3D<TScalar> result{};
			Vector3D<TScalar>::Store(result, DirectX::XMVector3TransformNormal(Vector3D<TScalar>::Load(vector), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TScalar>(
				vector.GetX() * (*this)[0] + vector.GetY() * (*this)[4] + vector.GetZ() * (*this)[8],
				vector.GetX() * (*this)[1] + vector.GetY() * (*this)[5] + vector.GetZ() * (*this)[9],
				vector.GetX() * (*this)[2] + vector.GetY() * (*this)[6] + vector.GetZ() * (*this)[10]);
		}
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::TryGetInverse(Matrix4x4<TScalar>& result) const noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMVECTOR determinant;
		const auto inverse = DirectX::XMMatrixInverse(&determinant, Load(*this));
		const float scalar = DirectX::XMVectorGetX(determinant);
		if (!std::isfinite(scalar) || scalar == 0.0f)
		{
			return false;
		}
		DirectX::XMFLOAT4X4 stored;
		DirectX::XMStoreFloat4x4(&stored, inverse);
		for (std::size_t row = 0; row < 4; ++row)
		{
			for (std::size_t column = 0; column < 4; ++column)
			{
				if (!std::isfinite(stored.m[row][column]))
				{
					return false;
				}
			}
		}
		result.value = stored;
		return true;
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::CanInverse() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(GetDeterminant()) > std::numeric_limits<float>::epsilon();
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::TryDecompose(Vector3D<float>& position, Quaternion<TScalar>& rotation, Vector3D<float>& scale) const noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMVECTOR scaleVec;
		DirectX::XMVECTOR rotVec;
		DirectX::XMVECTOR transVec;
		if (!DirectX::XMMatrixDecompose(&scaleVec, &rotVec, &transVec, Load(*this)))
		{
			return false;
		}
		Vector3D<float>::Store(position, transVec);
		Quaternion<TScalar>::Store(rotation, rotVec);
		Vector3D<float>::Store(scale, scaleVec);
		return true;
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::IsValidTRS() const noexcept
		requires std::same_as<TScalar, f32>
	{
		if (std::abs(value._14) > std::numeric_limits<float>::epsilon()
			|| std::abs(value._24) > std::numeric_limits<float>::epsilon()
			|| std::abs(value._34) > std::numeric_limits<float>::epsilon()
			|| std::abs(value._44 - 1.0f) > std::numeric_limits<float>::epsilon())
		{
			return false;
		}
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (!std::isfinite((*this)[index]))
			{
				return false;
			}
		}
		DirectX::XMVECTOR scale;
		DirectX::XMVECTOR rotation;
		DirectX::XMVECTOR translation;
		return DirectX::XMMatrixDecompose(&scale, &rotation, &translation, Load(*this));
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::IsIdentity(float epsilon) const noexcept
	{
		return IsApproximately(*this, GetIdentity(), epsilon);
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::GetIdentity() noexcept
	{
		return Matrix4x4<TScalar>();
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::GetZero() noexcept
	{
		return Matrix4x4<TScalar>(TScalar{});
	}

	template <ScalarType TScalar>
	DirectX::XMMATRIX Matrix4x4<TScalar>::Load(const Matrix4x4<TScalar>& matrix) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat4x4(&matrix.value);
	}

	template <ScalarType TScalar>
	void Matrix4x4<TScalar>::Store(Matrix4x4<TScalar>& destination, DirectX::XMMATRIX source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat4x4(&destination.value, source);
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::IsApproximately(const Matrix4x4<TScalar>& lhs, const Matrix4x4<TScalar>& rhs, float epsilon) noexcept
	{
		assert(epsilon >= 0.0f);
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (!(std::abs(static_cast<double>(lhs[index]) - static_cast<double>(rhs[index])) <= epsilon))
			{
				return false;
			}
		}
		return true;
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Translate(const Vector3D<TScalar>& translation) noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			return Matrix4x4<TScalar>(DirectX::XMMatrixTranslation(translation.GetX(), translation.GetY(), translation.GetZ()));
		}
		else
		{
			Matrix4x4<TScalar> result{};
			result[12] = translation.GetX();
			result[13] = translation.GetY();
			result[14] = translation.GetZ();
			return result;
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Rotate(const Quaternion<TScalar>& rotation) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixRotationQuaternion(Quaternion<TScalar>::Load(rotation)));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Scale(const Vector3D<TScalar>& scale) noexcept
	{
		if constexpr (std::same_as<TScalar, float>)
		{
			return Matrix4x4<TScalar>(DirectX::XMMatrixScaling(scale.GetX(), scale.GetY(), scale.GetZ()));
		}
		else
		{
			Matrix4x4<TScalar> result{};
			result[0] = scale.GetX();
			result[5] = scale.GetY();
			result[10] = scale.GetZ();
			return result;
		}
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::TRS(const Vector3D<float>& translation, const Quaternion<TScalar>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Scale(scale) * Rotate(rotation) * Translate(translation);
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::LookAt(const Vector3D<float>& from, const Vector3D<float>& to, const Vector3D<float>& up) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixLookAtLH(Vector3D<float>::Load(from), Vector3D<float>::Load(to), Vector3D<float>::Load(up)));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixPerspectiveFovLH(fovYDegrees * (std::numbers::pi_v<float> / 180.0f), aspect, nearZ, farZ));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Frustum(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearZ, farZ));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ));
	}

	template <ScalarType TScalar>
	Matrix4x4<TScalar> Matrix4x4<TScalar>::Ortho(float width, float height, float nearZ, float farZ) noexcept
		requires std::same_as<TScalar, f32>
	{
		return Matrix4x4<TScalar>(DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
	}

	template <ScalarType TScalar>
	bool Matrix4x4<TScalar>::TryInverse3DAffine(const Matrix4x4<TScalar>& input, Matrix4x4<TScalar>& result) noexcept
		requires std::same_as<TScalar, f32>
	{
		if (input.value._14 != 0.0f || input.value._24 != 0.0f
			|| input.value._34 != 0.0f || input.value._44 != 1.0f)
		{
			return false;
		}
		return input.TryGetInverse(result);
	}

	template class Matrix4x4<std::int32_t>;
	template class Matrix4x4<float>;
}
