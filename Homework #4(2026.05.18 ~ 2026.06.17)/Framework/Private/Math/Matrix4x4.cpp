#include "Precompiled.hpp"
#include "Matrix4x4.hpp"

#include "Quaternion.hpp"
#include "Matrix4x4_Impl.hpp"
#include "Quaternion_Impl.hpp"
#include "Vector3D_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4() noexcept
		: values{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = static_cast<TValue>(1);
		}
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4(TValue value) noexcept
		: values{}
	{
		for (std::size_t diagonal = 0; diagonal < 4; ++diagonal)
		{
			(*this)[diagonal * 5] = value;
		}
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4(const TValue values[16]) noexcept
		: values{}
	{
		assert(values);
		for (std::size_t index = 0; index < 16; ++index)
		{
			(*this)[index] = values[index];
		}
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4(
		TValue m00, TValue m01, TValue m02, TValue m03,
		TValue m10, TValue m11, TValue m12, TValue m13,
		TValue m20, TValue m21, TValue m22, TValue m23,
		TValue m30, TValue m31, TValue m32, TValue m33) noexcept
		: values{}
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

	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4(const Matrix4x4<TValue>& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 16; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>::Matrix4x4(Matrix4x4<TValue>&& other) noexcept
		: values{}
	{
		for (std::size_t index = 0; index < 16; ++index) values[index] = other.values[index];
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(const Matrix4x4<TValue>& other) noexcept
	{
		for (std::size_t index = 0; index < 16; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator=(Matrix4x4<TValue>&& other) noexcept
	{
		for (std::size_t index = 0; index < 16; ++index) values[index] = other.values[index];
		return *this;
	}

	template <IsNumeric TValue>
	TValue Matrix4x4<TValue>::operator[](size_t index) const noexcept
	{
		assert(index < 16);
		return values[index];
	}

	template <IsNumeric TValue>
	TValue& Matrix4x4<TValue>::operator[](size_t index) noexcept
	{
		assert(index < 16);
		return values[index];
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::operator*(const Matrix4x4<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Matrix4x4<TValue> result;
			Store(result, DirectX::XMMatrixMultiply(Load(*this), Load(other)));
			return result;
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

	template <IsNumeric TValue>
	Matrix4x4<TValue>& Matrix4x4<TValue>::operator*=(const Matrix4x4<TValue>& other) noexcept
	{
		*this = *this * other;
		return *this;
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::operator==(const Matrix4x4<TValue>& other) const noexcept
	{
		return (*this <=> other) == std::partial_ordering::equivalent;
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::operator!=(const Matrix4x4<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	Vector4D<TValue> Matrix4x4<TValue>::GetRow(std::size_t index) const noexcept
	{
		assert(index < 4);
		const auto base = index * 4;
		return Vector4D<TValue>((*this)[base], (*this)[base + 1], (*this)[base + 2], (*this)[base + 3]);
	}

	template <IsNumeric TValue>
	void Matrix4x4<TValue>::SetRow(std::size_t index, const Vector4D<TValue>& row) noexcept
	{
		assert(index < 4);
		for (std::size_t column = 0; column < 4; ++column)
		{
			(*this)[index * 4 + column] = row[column];
		}
	}

	template <IsNumeric TValue>
	Vector4D<TValue> Matrix4x4<TValue>::GetColumn(std::size_t index) const noexcept
	{
		assert(index < 4);
		return Vector4D<TValue>((*this)[index], (*this)[index + 4], (*this)[index + 8], (*this)[index + 12]);
	}

	template <IsNumeric TValue>
	void Matrix4x4<TValue>::SetColumn(std::size_t index, const Vector4D<TValue>& column) noexcept
	{
		assert(index < 4);
		for (std::size_t row = 0; row < 4; ++row)
		{
			(*this)[row * 4 + index] = column[row];
		}
	}

	template <IsNumeric TValue>
	float Matrix4x4<TValue>::GetDeterminant() const noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(Load(*this)));
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::GetTranspose() const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Matrix4x4<TValue> result;
			Store(result, DirectX::XMMatrixTranspose(Load(*this)));
			return result;
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

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::GetInverse() const noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4 result;
		const bool success = TryGetInverse(result);
		assert(success);
		if (!success)
			for (auto& component : result.values) component = std::numeric_limits<float>::quiet_NaN();
		return result;
	}

	template <IsNumeric TValue>
	Vector3D<TValue> Matrix4x4<TValue>::GetWorldPosition() const noexcept
	{
		return Vector3D<TValue>((*this)[12], (*this)[13], (*this)[14]);
	}

	template <IsNumeric TValue>
	Quaternion<TValue> Matrix4x4<TValue>::GetRotation() const noexcept
		requires std::same_as<TValue, float>
	{
		const auto scale = GetScale();
		if (scale.GetX() <= std::numeric_limits<float>::epsilon()
			|| scale.GetY() <= std::numeric_limits<float>::epsilon()
			|| scale.GetZ() <= std::numeric_limits<float>::epsilon()) return Quaternion<TValue>::GetIdentity();
		const float m00 = values[0] / scale.GetX(), m01 = values[1] / scale.GetX(), m02 = values[2] / scale.GetX();
		const float m10 = values[4] / scale.GetY(), m11 = values[5] / scale.GetY(), m12 = values[6] / scale.GetY();
		const float m20 = values[8] / scale.GetZ(), m21 = values[9] / scale.GetZ(), m22 = values[10] / scale.GetZ();
		const float trace = m00 + m11 + m22;
		if (trace > 0.0f)
		{
			const float s = 2.0f * std::sqrt(1.0f + trace);
			return Quaternion<TValue>((m12 - m21) / s, (m20 - m02) / s, (m01 - m10) / s, s * 0.25f);
		}
		if (m00 >= m11 && m00 >= m22)
		{
			const float s = 2.0f * std::sqrt(1.0f + m00 - m11 - m22);
			return Quaternion<TValue>(s * 0.25f, (m01 + m10) / s, (m02 + m20) / s, (m12 - m21) / s);
		}
		if (m11 >= m22)
		{
			const float s = 2.0f * std::sqrt(1.0f + m11 - m00 - m22);
			return Quaternion<TValue>((m01 + m10) / s, s * 0.25f, (m12 + m21) / s, (m20 - m02) / s);
		}
		const float s = 2.0f * std::sqrt(1.0f + m22 - m00 - m11);
		return Quaternion<TValue>((m02 + m20) / s, (m12 + m21) / s, s * 0.25f, (m01 - m10) / s);
	}

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::GetScale() const noexcept
		requires std::same_as<TValue, float>
	{
		const Vector3D<float> xAxis(values[0], values[1], values[2]);
		const Vector3D<float> yAxis(values[4], values[5], values[6]);
		const Vector3D<float> zAxis(values[8], values[9], values[10]);
		return Vector3D<float>(xAxis.GetMagnitude(), yAxis.GetMagnitude(), zAxis.GetMagnitude());
	}

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::GetLossyScale() const noexcept
		requires std::same_as<TValue, float>
	{
		return GetScale();
	}

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::GetForward() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(values[8], values[9], values[10]).GetNormalized();
	}

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::GetUp() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(values[4], values[5], values[6]).GetNormalized();
	}

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::GetRight() const noexcept
		requires std::same_as<TValue, float>
	{
		return Vector3D<float>(values[0], values[1], values[2]).GetNormalized();
	}

	template <IsNumeric TValue>
	void Matrix4x4<TValue>::SetTRS(const Vector3D<float>& position, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		*this = TRS(position, rotation, scale);
	}

	template <IsNumeric TValue>
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyPoint3x4(const Vector3D<TValue>& point) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result;
			Store(result, DirectX::XMVector3Transform(Load(point), Load(*this)));
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

	template <IsNumeric TValue>
	Vector3D<float> Matrix4x4<TValue>::MultiplyPoint(const Vector3D<float>& point) const noexcept
		requires std::same_as<TValue, float>
	{
		Vector3D<float> result;
		Store(result, DirectX::XMVector3TransformCoord(Load(point), Load(*this)));
		return result;
	}

	template <IsNumeric TValue>
	Vector3D<TValue> Matrix4x4<TValue>::MultiplyVector(const Vector3D<TValue>& vector) const noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Vector3D<TValue> result;
			Store(result, DirectX::XMVector3TransformNormal(Load(vector), Load(*this)));
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

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::TryGetInverse(Matrix4x4<TValue>& result) const noexcept
		requires std::same_as<TValue, float>
	{
		// Partial pivoting also handles invertible matrices with a zero diagonal.
		double augmented[4][8]{};
		for (std::size_t row = 0; row < 4; ++row)
		{
			for (std::size_t column = 0; column < 4; ++column)
			{
				if (!std::isfinite((*this)[row * 4 + column])) return false;
				augmented[row][column] = (*this)[row * 4 + column];
			}
			augmented[row][row + 4] = 1.0;
		}
		for (std::size_t column = 0; column < 4; ++column)
		{
			std::size_t pivot = column;
			for (std::size_t row = column + 1; row < 4; ++row)
				if (std::abs(augmented[row][column]) > std::abs(augmented[pivot][column])) pivot = row;
			if (augmented[pivot][column] == 0.0) return false;
			if (pivot != column)
				for (std::size_t i = 0; i < 8; ++i) std::swap(augmented[pivot][i], augmented[column][i]);
			const double divisor = augmented[column][column];
			for (double& component : augmented[column]) component /= divisor;
			for (std::size_t row = 0; row < 4; ++row)
			{
				if (row == column) continue;
				const double factor = augmented[row][column];
				for (std::size_t i = 0; i < 8; ++i) augmented[row][i] -= factor * augmented[column][i];
			}
		}
		Matrix4x4 inverse;
		for (std::size_t row = 0; row < 4; ++row)
			for (std::size_t column = 0; column < 4; ++column)
			{
				const double component = augmented[row][column + 4];
				if (!std::isfinite(component) || std::abs(component) > std::numeric_limits<float>::max()) return false;
				inverse[row * 4 + column] = static_cast<float>(component);
			}
		result = inverse;
		return true;
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::TryDecompose(Vector3D<float>& position, Quaternion<TValue>& rotation, Vector3D<float>& scale) const noexcept
		requires std::same_as<TValue, float>
	{
		constexpr float tolerance = 0.0001f;
		for (const auto component : values)
		{
			if (!std::isfinite(component)) return false;
		}
		if (std::abs(values[3]) > tolerance || std::abs(values[7]) > tolerance
			|| std::abs(values[11]) > tolerance || std::abs(values[15] - 1.0f) > tolerance) return false;

		Vector3D<float> axes[3]{
			{values[0], values[1], values[2]},
			{values[4], values[5], values[6]},
			{values[8], values[9], values[10]}
		};
		float lengths[3]{axes[0].GetMagnitude(), axes[1].GetMagnitude(), axes[2].GetMagnitude()};
		for (float length : lengths)
		{
			if (!std::isfinite(length)) return false;
		}
		// Complete missing axes when one or more scale components are zero.
		std::size_t order[3]{0, 1, 2};
		std::sort(std::begin(order), std::end(order),
			[&](std::size_t a, std::size_t b) { return lengths[a] == lengths[b] ? a < b : lengths[a] > lengths[b]; });
		const auto a = order[0], b = order[1], c = order[2];
		const Vector3D<float> canonical[3]{Vector3D<float>::GetRight(), Vector3D<float>::GetUp(), Vector3D<float>::GetForward()};
		if (lengths[a] < tolerance) axes[a] = canonical[a];
		axes[a].Normalize();
		if (lengths[b] < tolerance)
		{
			std::size_t leastAligned = 0;
			for (std::size_t i = 1; i < 3; ++i)
				if (std::abs(axes[a][i]) < std::abs(axes[a][leastAligned])) leastAligned = i;
			axes[b] = Vector3D<float>::Cross(axes[a], canonical[leastAligned]);
		}
		axes[b].Normalize();
		if (lengths[c] < tolerance) axes[c] = Vector3D<float>::Cross(axes[a], axes[b]);
		axes[c].Normalize();
		float determinant = Vector3D<float>::Dot(axes[0], Vector3D<float>::Cross(axes[1], axes[2]));
		if (determinant < 0.0f)
		{
			lengths[a] = -lengths[a];
			axes[a] = -axes[a];
			determinant = -determinant;
		}
		if (!std::isfinite(determinant) || (determinant - 1.0f) * (determinant - 1.0f) > tolerance) return false;
		Matrix4x4 basis;
		for (std::size_t row = 0; row < 3; ++row) basis.SetRow(row, Vector4D<float>(axes[row]));
		const auto candidateRotation = basis.GetRotation().GetNormalized();
		if (!candidateRotation.IsFinite()) return false;
		position = GetWorldPosition();
		rotation = candidateRotation;
		scale.Set(lengths[0], lengths[1], lengths[2]);
		return true;
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::CanInverse() const noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(GetDeterminant()) > std::numeric_limits<float>::epsilon();
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::IsValidTRS() const noexcept
		requires std::same_as<TValue, float>
	{
		constexpr float epsilon = std::numeric_limits<float>::epsilon();
		if (std::abs(values[3]) > epsilon || std::abs(values[7]) > epsilon
			|| std::abs(values[11]) > epsilon || std::abs(values[15] - 1.0f) > epsilon) return false;
		Vector3D<float> position, scale;
		Quaternion<TValue> rotation;
		return TryDecompose(position, rotation, scale);
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::IsIdentity(float epsilon) const noexcept
	{
		return IsApproximately(*this, GetIdentity(), epsilon);
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::GetIdentity() noexcept
	{
		return Matrix4x4<TValue>();
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::GetZero() noexcept
	{
		return Matrix4x4<TValue>(TValue{});
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Translate(const Vector3D<TValue>& translation) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Matrix4x4<TValue> result;
			Store(result, DirectX::XMMatrixTranslationFromVector(Load(translation)));
			return result;
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

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Rotate(const Quaternion<TValue>& rotation) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixRotationQuaternion(Load(rotation)));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Scale(const Vector3D<TValue>& scale) noexcept
	{
		if constexpr (std::same_as<TValue, float>)
		{
			Matrix4x4<TValue> result;
			Store(result, DirectX::XMMatrixScalingFromVector(Load(scale)));
			return result;
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

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::TRS(const Vector3D<float>& translation, const Quaternion<TValue>& rotation, const Vector3D<float>& scale) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixAffineTransformation(Load(scale), DirectX::XMVectorZero(), Load(rotation), Load(translation)));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::LookAt(const Vector3D<float>& from, const Vector3D<float>& to, const Vector3D<float>& up) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixLookAtLH(Load(from), Load(to), Load(up)));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Perspective(float fovYDegrees, float aspect, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fovYDegrees), aspect, nearZ, farZ));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Frustum(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearZ, farZ));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float left, float right, float bottom, float top, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixOrthographicOffCenterLH(left, right, bottom, top, nearZ, farZ));
		return result;
	}

	template <IsNumeric TValue>
	Matrix4x4<TValue> Matrix4x4<TValue>::Ortho(float width, float height, float nearZ, float farZ) noexcept
		requires std::same_as<TValue, float>
	{
		Matrix4x4<TValue> result;
		Store(result, DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));
		return result;
	}

	template <IsNumeric TValue>
	bool Matrix4x4<TValue>::TryInverse3DAffine(const Matrix4x4<TValue>& input, Matrix4x4<TValue>& result) noexcept
		requires std::same_as<TValue, float>
	{
		if (input.values[3] != 0.0f || input.values[7] != 0.0f
			|| input.values[11] != 0.0f || input.values[15] != 1.0f)
		{
			return false;
		}
		return input.TryGetInverse(result);
	}

	template class Matrix4x4<int>;
	template class Matrix4x4<float>;
}
