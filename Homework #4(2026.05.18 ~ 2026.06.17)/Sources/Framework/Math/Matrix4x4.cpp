#include "Precompiled.h"
#include "Matrix4x4.h"

#include "Quaternion.h"

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4() noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = TValue{1};
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(TValue value_) noexcept
		: value{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = value_;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(const TValue values_[16]) noexcept
		: value{}
	{
		assert(values_);
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = values_[index];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(
		TValue m00_, TValue m01_, TValue m02_, TValue m03_,
		TValue m10_, TValue m11_, TValue m12_, TValue m13_,
		TValue m20_, TValue m21_, TValue m22_, TValue m23_,
		TValue m30_, TValue m31_, TValue m32_, TValue m33_) noexcept
		: value{}
	{
		const TValue components[16]{
			m00_, m01_, m02_, m03_,
			m10_, m11_, m12_, m13_,
			m20_, m21_, m22_, m23_,
			m30_, m31_, m32_, m33_
		};
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = components[index];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(const Matrix4x4<TValue>& other_) noexcept
		: value(other_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(Matrix4x4<TValue>&& other_) noexcept
		: value(other_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>::Matrix4x4(DirectX::XMMATRIX matrix_) noexcept
		requires std::same_as<TValue, float>
		: value{}
	{
		Store(*this, matrix_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(const Matrix4x4<TValue>& other_) noexcept
	{
		value = other_.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(Matrix4x4<TValue>&& other_) noexcept
	{
		value = other_.value;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue Matrix4x4<TValue>::operator[](size_t index_) const noexcept
	{
		assert(index_ < 16);
		if constexpr (std::same_as<TValue, int>)
		{
			return value[index_];
		}
		else
		{
			return value.m[index_ / 4][index_ % 4];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue& Matrix4x4<TValue>::operator[](size_t index_) noexcept
	{
		assert(index_ < 16);
		if constexpr (std::same_as<TValue, int>)
		{
			return value[index_];
		}
		else
		{
			return value.m[index_ / 4][index_ % 4];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::operator*(const Matrix4x4<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixMultiply(Load(*this), Load(other_)));
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
						result[row * 4 + column] += (*this)[row * 4 + element] * other_[element * 4 + column];
					}
				}
			}
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator*=(const Matrix4x4<TValue>& other_) noexcept
	{
		*this = *this * other_;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::operator==(const Matrix4x4<TValue>& other_) const noexcept
	{
		return (*this <=> other_) == std::partial_ordering::equivalent;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::operator!=(const Matrix4x4<TValue>& other_) const noexcept
	{
		return !(*this == other_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	std::partial_ordering Matrix4x4<TValue>::operator<=>(const Matrix4x4<TValue>& other_) const noexcept
	{
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (const auto order = (*this)[index] <=> other_[index]; order != 0)
			{
				return order;
			}
		}
		return std::partial_ordering::equivalent;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Matrix4x4<TValue>::GetRow(std::size_t index_) const noexcept
	{
		assert(index_ < 4);
		const auto base = index_ * 4;
		return Vector4D<TValue>((*this)[base], (*this)[base + 1], (*this)[base + 2], (*this)[base + 3]);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::SetRow(std::size_t index_, const Vector4D<TValue>& row_) noexcept
	{
		assert(index_ < 4);
		for (std::size_t column = 0; column < 4; ++column)
		{
			(*this)[index_ * 4 + column] = row_[column];
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> Matrix4x4<TValue>::GetColumn(std::size_t index_) const noexcept
	{
		assert(index_ < 4);
		return Vector4D<TValue>((*this)[index_], (*this)[index_ + 4], (*this)[index_ + 8], (*this)[index_ + 12]);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::SetColumn(std::size_t index_, const Vector4D<TValue>& column_) noexcept
	{
		assert(index_ < 4);
		for (std::size_t row = 0; row < 4; ++row)
		{
			(*this)[row * 4 + index_] = column_[row];
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
		const Vector3D<float> scale{ GetScale() };
		if (scale.GetX() <= std::numeric_limits<float>::epsilon() || scale.GetY() <= std::numeric_limits<float>::epsilon() || scale.GetZ() <= std::numeric_limits<float>::epsilon())
		{
			return Quaternion<TValue>::GetIdentity();
		}

		DirectX::XMMATRIX rotation{ Load(*this) };
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
		const Vector3D<float> xAxis{ value._11, value._12, value._13 };
		const Vector3D<float> yAxis{ value._21, value._22, value._23 };
		const Vector3D<float> zAxis{ value._31, value._32, value._33 };
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
	void Matrix4x4<TValue>::SetTRS(const Vector3D<float>& position_, const Quaternion<TValue>& rotation_, const Vector3D<float>& scale_) noexcept
		requires std::same_as<TValue, float>
	{
		*this = TRS(position_, rotation_, scale_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyPoint3x4(const Vector3D<TValue>& point_) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result;
			Vector3D<TValue>::Store(result, DirectX::XMVector3Transform(Vector3D<TValue>::Load(point_), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TValue>(
				point_.GetX() * (*this)[0] + point_.GetY() * (*this)[4] + point_.GetZ() * (*this)[8] + (*this)[12],
				point_.GetX() * (*this)[1] + point_.GetY() * (*this)[5] + point_.GetZ() * (*this)[9] + (*this)[13],
				point_.GetX() * (*this)[2] + point_.GetY() * (*this)[6] + point_.GetZ() * (*this)[10] + (*this)[14]);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<float> Matrix4x4<TValue>::MultiplyPoint(const Vector3D<float>& point_) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result{};
		Vector3D<float>::Store(result, DirectX::XMVector3TransformCoord(Vector3D<float>::Load(point_), Load(*this)));
		return result;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyVector(const Vector3D<TValue>& vector_) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result;
			Vector3D<TValue>::Store(result, DirectX::XMVector3TransformNormal(Vector3D<TValue>::Load(vector_), Load(*this)));
			return result;
		}
		else
		{
			return Vector3D<TValue>(
				vector_.GetX() * (*this)[0] + vector_.GetY() * (*this)[4] + vector_.GetZ() * (*this)[8],
				vector_.GetX() * (*this)[1] + vector_.GetY() * (*this)[5] + vector_.GetZ() * (*this)[9],
				vector_.GetX() * (*this)[2] + vector_.GetY() * (*this)[6] + vector_.GetZ() * (*this)[10]);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::TryGetInverse(Matrix4x4<TValue>& result_) const noexcept
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
		result_.value = stored;
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
	bool Matrix4x4<TValue>::IsIdentity(float epsilon_) const noexcept
	{
		return IsApproximately(*this, GetIdentity(), epsilon_);
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
	DirectX::XMMATRIX Matrix4x4<TValue>::Load(const Matrix4x4<TValue>& matrix_) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat4x4(&matrix_.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void Matrix4x4<TValue>::Store(Matrix4x4<TValue>& destination_, DirectX::XMMATRIX source_) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat4x4(&destination_.value, source_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::IsApproximately(const Matrix4x4<TValue>& lhs_, const Matrix4x4<TValue>& rhs_, float epsilon_) noexcept
	{
		assert(epsilon_ >= 0.0f);
		for (std::size_t index = 0; index < 16; ++index)
		{
			if (!(std::abs(static_cast<double>(lhs_[index]) - static_cast<double>(rhs_[index])) <= epsilon_))
			{
				return false;
			}
		}
		return true;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Translate(const Vector3D<TValue>& translation_) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixTranslation(translation_.GetX(), translation_.GetY(), translation_.GetZ()));
		}
		else
		{
			Matrix4x4<TValue> result;
			result[12] = translation_.GetX();
			result[13] = translation_.GetY();
			result[14] = translation_.GetZ();
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Rotate(const Quaternion<TValue>& rotation_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixRotationQuaternion(Quaternion<TValue>::Load(rotation_)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Scale(const Vector3D<TValue>& scale_) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			return Matrix4x4<TValue>(DirectX::XMMatrixScaling(scale_.GetX(), scale_.GetY(), scale_.GetZ()));
		}
		else
		{
			Matrix4x4<TValue> result;
			result[0] = scale_.GetX();
			result[5] = scale_.GetY();
			result[10] = scale_.GetZ();
			return result;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::TRS(const Vector3D<float>& translation_, const Quaternion<TValue>& rotation_, const Vector3D<float>& scale_) noexcept
		requires std::same_as<TValue, float>
	{
		return Scale(scale_) * Rotate(rotation_) * Translate(translation_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::LookAt(const Vector3D<float>& from_, const Vector3D<float>& to_, const Vector3D<float>& up_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixLookAtLH(Vector3D<float>::Load(from_), Vector3D<float>::Load(to_), Vector3D<float>::Load(up_)));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Perspective(float fovYDegrees_, float aspect_, float nearZ_, float farZ_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixPerspectiveFovLH(fovYDegrees_ * (std::numbers::pi_v<float> / 180.0f), aspect_, nearZ_, farZ_));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Frustum(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixPerspectiveOffCenterLH(left_, right_, bottom_, top_, nearZ_, farZ_));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float left_, float right_, float bottom_, float top_, float nearZ_, float farZ_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixOrthographicOffCenterLH(left_, right_, bottom_, top_, nearZ_, farZ_));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float width_, float height_, float nearZ_, float farZ_) noexcept
		requires std::same_as<TValue, float>
	{
		return Matrix4x4<TValue>(DirectX::XMMatrixOrthographicLH(width_, height_, nearZ_, farZ_));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool Matrix4x4<TValue>::TryInverse3DAffine(const Matrix4x4<TValue>& input_, Matrix4x4<TValue>& result_) noexcept
		requires std::same_as<TValue, float>
	{
		if (input_.value._14 != 0.0f || input_.value._24 != 0.0f
			|| input_.value._34 != 0.0f || input_.value._44 != 1.0f)
		{
			return false;
		}
		return input_.TryGetInverse(result_);
	}

	template class Matrix4x4<int>;
	template class Matrix4x4<float>;
}
