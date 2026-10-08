#include "Precompiled.hpp"
#include "Matrix4x4.hpp"

#include "Quaternion.hpp"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4() noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = static_cast<TValue>(1);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(TValue value) noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = value;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(const TValue values[16]) noexcept
		: value{}
	{
		assert(values);
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = values[index];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(
		TValue m00, TValue m01, TValue m02, TValue m03,
		TValue m10, TValue m11, TValue m12, TValue m13,
		TValue m20, TValue m21, TValue m22, TValue m23,
		TValue m30, TValue m31, TValue m32, TValue m33) noexcept
		: value{}
	{
		const TValue components[16]{
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(const Matrix4x4<TValue>& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(Matrix4x4<TValue>&& other) noexcept
		: value(other.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(DirectX::XMMATRIX matrix) noexcept
		requires std::same_as<TValue, float>
		: value{}
	{
		Store(*this, matrix);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(const Matrix4x4<TValue>& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(Matrix4x4<TValue>&& other) noexcept
	{
		value = other.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Matrix4x4<TValue>::operator[](size_t index) const noexcept
	{
		assert(index < 16);
		if constexpr (std::same_as<TValue, int>)
		{
			return value[index];
		}
		else
		{
			return value.m[index / 4][index % 4];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue& Matrix4x4<TValue>::operator[](size_t index) noexcept
	{
		assert(index < 16);
		if constexpr (std::same_as<TValue, int>)
		{
			return value[index];
		}
		else
		{
			return value.m[index / 4][index % 4];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::operator*(const Matrix4x4<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixMultiply(Load(*this), Load(other)));
		}
		else
		{
			Matrix4x4<TValue> result(TValue{});
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator*=(const Matrix4x4<TValue>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::operator==(const Matrix4x4<TValue>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::operator!=(const Matrix4x4<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Matrix4x4<TValue>::operator<=>(const Matrix4x4<TValue>& other) const noexcept
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Matrix4x4<TValue>::GetRow(std::size_t index) const noexcept
	{
		assert(index < 4);
		const auto base = index * 4;
		return Vector4D<TValue>((*this)[base], (*this)[base + 1], (*this)[base + 2], (*this)[base + 3]);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::SetRow(std::size_t index, const Vector4D<TValue>& row) noexcept
	{
		assert(index < 4);
		for (std::size_t column = 0; column < 4; ++column)
		{
			(*this)[index * 4 + column] = row[column];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Matrix4x4<TValue>::GetColumn(std::size_t index) const noexcept
	{
		assert(index < 4);
		return Vector4D<TValue>((*this)[index], (*this)[index + 4], (*this)[index + 8], (*this)[index + 12]);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::SetColumn(std::size_t index, const Vector4D<TValue>& column) noexcept
	{
		assert(index < 4);
		for (std::size_t row = 0; row < 4; ++row)
		{
			(*this)[row * 4 + index] = column[row];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float Matrix4x4<TValue>::GetDeterminant() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(Load(*this)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::GetTranspose() const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixTranspose(Load(*this)));
		}
		else
		{
			Matrix4x4<TValue> result(TValue{});
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::GetInverse() const noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMVECTOR determinant;
		const auto inverse = DirectX::XMMatrixInverse(&determinant, Load(*this));
		const float scalar = DirectX::XMVectorGetX(determinant);
		assert(std::isfinite(scalar) && scalar != 0.0f);
		return Matrix4x4<TValue>(inverse);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Matrix4x4<TValue>::GetWorldPosition() const noexcept
	{
		return Vector3D<TValue>((*this)[12], (*this)[13], (*this)[14]);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Quaternion<TValue> Matrix4x4<TValue>::GetRotation() const noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<float> scale = GetScale();
		if (scale.GetX() <= std::numeric_limits<float>::epsilon() || scale.GetY() <= std::numeric_limits<float>::epsilon() || scale.GetZ() <= std::numeric_limits<float>::epsilon())
		{
			return Quaternion<TValue>::GetIdentity();
		}

		DirectX::XMMATRIX rotation = Load(*this);
		rotation.r[0] = DirectX::XMVectorScale(rotation.r[0], 1.0f / scale.GetX());
		rotation.r[1] = DirectX::XMVectorScale(rotation.r[1], 1.0f / scale.GetY());
		rotation.r[2] = DirectX::XMVectorScale(rotation.r[2], 1.0f / scale.GetZ());
		rotation.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		Quaternion<TValue> result{};
		Quaternion<TValue>::Store(result, DirectX::XMQuaternionRotationMatrix(rotation));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::GetScale() const noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<float> xAxis(value._11, value._12, value._13);
		const Vector3D<float> yAxis(value._21, value._22, value._23);
		const Vector3D<float> zAxis(value._31, value._32, value._33);
		return Vector3D<float>(xAxis.GetMagnitude(), yAxis.GetMagnitude(), zAxis.GetMagnitude());
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::GetLossyScale() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetScale();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::GetForward() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(value._31, value._32, value._33).GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::GetUp() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(value._21, value._22, value._23).GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::GetRight() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(value._11, value._12, value._13).GetNormalized();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::SetTRS(const Vector3D<float>& position, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		*this = TRS(position, rotation, scale);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyPoint3x4(const Vector3D<TValue>& point) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result{};
			Vector3D<TValue>::Store(result, DirectX::XMVector3Transform(Vector3D<TValue>::Load(point), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TValue>(
				point.GetX() * (*this)[0] + point.GetY() * (*this)[4] + point.GetZ() * (*this)[8] + (*this)[12],
				point.GetX() * (*this)[1] + point.GetY() * (*this)[5] + point.GetZ() * (*this)[9] + (*this)[13],
				point.GetX() * (*this)[2] + point.GetY() * (*this)[6] + point.GetZ() * (*this)[10] + (*this)[14]);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::MultiplyPoint(const Vector3D<float>& point) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result{};
		Vector3D<float>::Store(result, DirectX::XMVector3TransformCoord(Vector3D<float>::Load(point), Load(*this)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyVector(const Vector3D<TValue>& vector) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result{};
			Vector3D<TValue>::Store(result, DirectX::XMVector3TransformNormal(Vector3D<TValue>::Load(vector), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TValue>(
				vector.GetX() * (*this)[0] + vector.GetY() * (*this)[4] + vector.GetZ() * (*this)[8],
				vector.GetX() * (*this)[1] + vector.GetY() * (*this)[5] + vector.GetZ() * (*this)[9],
				vector.GetX() * (*this)[2] + vector.GetY() * (*this)[6] + vector.GetZ() * (*this)[10]);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::TryGetInverse(Matrix4x4<TValue>& result) const noexcept
		requires std::same_as<TValue, float>
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::CanInverse() const noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(GetDeterminant()) > std::numeric_limits<float>::epsilon();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::IsValidTRS() const noexcept
		requires std::same_as<TValue, float>
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::IsIdentity(float epsilon) const noexcept
	{
		return IsApproximately(*this, GetIdentity(), epsilon);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::GetIdentity() noexcept
	{
		return Matrix4x4<TValue>();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::GetZero() noexcept
	{
		return Matrix4x4<TValue>(TValue{});
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMMATRIX Matrix4x4<TValue>::Load(const Matrix4x4<TValue>& matrix) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4x4(&matrix.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::Store(Matrix4x4<TValue>& destination, DirectX::XMMATRIX source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4x4(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::IsApproximately(const Matrix4x4<TValue>& lhs, const Matrix4x4<TValue>& rhs, float epsilon) noexcept
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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Translate(const Vector3D<TValue>& translation) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixTranslation(translation.GetX(), translation.GetY(), translation.GetZ()));
		}
		else
		{
			Matrix4x4<TValue> result{};
			result[12] = translation.GetX();
			result[13] = translation.GetY();
			result[14] = translation.GetZ();
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Rotate(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixRotationQuaternion(Quaternion<TValue>::Load(rotation)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Scale(const Vector3D<TValue>& scale) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixScaling(scale.GetX(), scale.GetY(), scale.GetZ()));
		}
		else
		{
			Matrix4x4<TValue> result{};
			result[0] = scale.GetX();
			result[5] = scale.GetY();
			result[10] = scale.GetZ();
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::TRS(const Vector3D<float>& translation, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		return Scale(scale) * Rotate(rotation) * Translate(translation);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::LookAt(const Vector3D<float>& from, const Vector3D<float>& to, const Vector3D<float>& up) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixLookAtLH(Vector3D<float>::Load(from), Vector3D<float>::Load(to), Vector3D<float>::Load(up)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixPerspectiveFovLH(fovYDegrees * (std::numbers::pi_v<float> / 180.0f), aspect, nearZ, farZ));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Frustum(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearZ, farZ));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float width, float height, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::TryInverse3DAffine(const Matrix4x4<TValue>& input, Matrix4x4<TValue>& result) noexcept
		requires std::same_as<TValue, float>
	{
		if (input.value._14 != 0.0f || input.value._24 != 0.0f
			|| input.value._34 != 0.0f || input.value._44 != 1.0f)
		{
			return false;
		}
		return input.TryGetInverse(result);
	}

	template class Matrix4x4<int>;
	template class Matrix4x4<float>;
}
