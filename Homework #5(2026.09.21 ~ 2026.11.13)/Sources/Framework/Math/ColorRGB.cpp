#include "Precompiled.hpp"
#include "ColorRGB.hpp"

#include "ColorRGBA.hpp"
#include "Mathf.hpp"

namespace TUK::Framework
{
	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB() noexcept
		: value(static_cast<TScalar>(0), static_cast<TScalar>(0), static_cast<TScalar>(0))
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(TScalar value) noexcept
		: value(value, value, value)
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(TScalar r, TScalar g, TScalar b) noexcept
		: value(r, g, b)
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(const Vector3D<TScalar>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ())
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(const Vector4D<TScalar>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ())
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(const ColorRGBA<TScalar>& color) noexcept
		: value(color.GetR(), color.GetG(), color.GetB())
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(DirectX::XMVECTOR vector) noexcept
		requires std::same_as<TScalar, f32>
		: value(TScalar{}, TScalar{}, TScalar{})
	{
		Store(*this, vector);
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(const DirectX::XMFLOAT3& source) noexcept
		requires std::same_as<TScalar, f32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::ColorRGB(const DirectX::XMINT3& source) noexcept
		requires std::same_as<TScalar, i32>
		: value(source)
	{
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::operator DirectX::XMVECTOR() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return Load(*this);
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::operator DirectX::XMFLOAT3() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return value;
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar>::operator DirectX::XMINT3() const noexcept
		requires std::same_as<TScalar, i32>
	{
		return value;
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::operator==(const ColorRGB<TScalar>& other) const noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return value.x == other.value.x && value.y == other.value.y && value.z == other.value.z;
		}
		else
		{
			return IsApproximately(*this, other);
		}
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::operator!=(const ColorRGB<TScalar>& other) const noexcept
	{
		return !(*this == other);
	}

	template <Scalarable TScalar>
	TScalar ColorRGB<TScalar>::GetR() const noexcept
	{
		return value.x;
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::SetR(TScalar component) noexcept
	{
		value.x = component;
	}

	template <Scalarable TScalar>
	TScalar ColorRGB<TScalar>::GetG() const noexcept
	{
		return value.y;
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::SetG(TScalar component) noexcept
	{
		value.y = component;
	}

	template <Scalarable TScalar>
	TScalar ColorRGB<TScalar>::GetB() const noexcept
	{
		return value.z;
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::SetB(TScalar component) noexcept
	{
		value.z = component;
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::Set(TScalar r, TScalar g, TScalar b) noexcept
	{
		value.x = r;
		value.y = g;
		value.z = b;
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetGamma() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return ColorRGB<TScalar>(
			LinearToGammaSpace(value.x),
			LinearToGammaSpace(value.y),
			LinearToGammaSpace(value.z));
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetLinear() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return ColorRGB<TScalar>(
			GammaToLinearSpace(value.x),
			GammaToLinearSpace(value.y),
			GammaToLinearSpace(value.z));
	}

	template <Scalarable TScalar>
	float ColorRGB<TScalar>::GetGrayscale() const noexcept
		requires std::same_as<TScalar, f32>
	{
		return 0.299f * value.x + 0.587f * value.y + 0.114f * value.z;
	}

	template <Scalarable TScalar>
	TScalar ColorRGB<TScalar>::GetMaxColorComponent() const noexcept
	{
		return std::max(value.x, std::max(value.y, value.z));
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::IsFinite() const noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::IsHDR() const noexcept
	{
		constexpr TScalar maximum = static_cast<TScalar>(std::same_as<TScalar, std::int32_t> ? 255 : 1);
		return value.x > maximum || value.y > maximum || value.z > maximum;
	}

	template <Scalarable TScalar>
	Vector3D<TScalar> ColorRGB<TScalar>::ToVector3D() const noexcept
	{
		return Vector3D<TScalar>(value.x, value.y, value.z);
	}

	template <Scalarable TScalar>
	Vector4D<TScalar> ColorRGB<TScalar>::ToVector4D(TScalar alpha) const noexcept
	{
		return Vector4D<TScalar>(value.x, value.y, value.z, alpha);
	}

	template <Scalarable TScalar>
	ColorRGBA<TScalar> ColorRGB<TScalar>::ToColorRGBA(TScalar alpha) const noexcept
	{
		return ColorRGBA<TScalar>(*this, alpha);
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetBlack() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(0, 0, 0);
		}
		else
		{
			return ColorRGB<TScalar>(0.0f, 0.0f, 0.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetWhite() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(255, 255, 255);
		}
		else
		{
			return ColorRGB<TScalar>(1.0f, 1.0f, 1.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetRed() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(255, 0, 0);
		}
		else
		{
			return ColorRGB<TScalar>(1.0f, 0.0f, 0.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetGreen() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(0, 255, 0);
		}
		else
		{
			return ColorRGB<TScalar>(0.0f, 1.0f, 0.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetBlue() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(0, 0, 255);
		}
		else
		{
			return ColorRGB<TScalar>(0.0f, 0.0f, 1.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetYellow() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(255, 255, 0);
		}
		else
		{
			return ColorRGB<TScalar>(1.0f, 1.0f, 0.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetCyan() noexcept
	{
		if constexpr (std::same_as<TScalar, std::int32_t>)
		{
			return ColorRGB<TScalar>(0, 255, 255);
		}
		else
		{
			return ColorRGB<TScalar>(0.0f, 1.0f, 1.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetMagenta() noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return ColorRGB<TScalar>(255, 0, 255);
		}
		else
		{
			return ColorRGB<TScalar>(1.0f, 0.0f, 1.0f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetGray() noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			return ColorRGB<TScalar>(128, 128, 128);
		}
		else
		{
			return ColorRGB<TScalar>(0.5f, 0.5f, 0.5f);
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::GetGrey() noexcept
	{
		return GetGray();
	}

	template <Scalarable TScalar>
	DirectX::XMVECTOR ColorRGB<TScalar>::Load(const ColorRGB<TScalar>& color) noexcept
		requires std::same_as<TScalar, f32>
	{
		return DirectX::XMLoadFloat3(&color.value);
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::Store(ColorRGB<TScalar>& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TScalar, f32>
	{
		DirectX::XMStoreFloat3(&destination.value, source);
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::IsApproximately(const ColorRGB<TScalar>& lhs, const ColorRGB<TScalar>& rhs, float epsilon) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::abs(lhs.value.x - rhs.value.x) <= epsilon
		&& std::abs(lhs.value.y - rhs.value.y) <= epsilon
		&& std::abs(lhs.value.z - rhs.value.z) <= epsilon;
	}

	template <Scalarable TScalar>
	float ColorRGB<TScalar>::LinearToGammaSpace(float value) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::pow(std::max(0.0f, value), 1.0f / 2.2f);
	}

	template <Scalarable TScalar>
	float ColorRGB<TScalar>::GammaToLinearSpace(float value) noexcept
		requires std::same_as<TScalar, f32>
	{
		return std::pow(std::max(0.0f, value), 2.2f);
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::HSVToRGB(float h, float s, float v) noexcept
		requires std::same_as<TScalar, f32>
	{
		return HSVToRGB(h, s, v, false);
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::HSVToRGB(float h, float s, float v, bool hdr) noexcept
		requires std::same_as<TScalar, f32>
	{
		h = h - std::floor(h);
		s = std::clamp(s, 0.0f, 1.0f);
		if (!hdr)
		{
			v = std::clamp(v, 0.0f, 1.0f);
		}

		if (s <= std::numeric_limits<float>::epsilon())
		{
			return ColorRGB<TScalar>(v, v, v);
		}

		const float scaledH = h * 6.0f;
		const int sector = static_cast<int>(std::floor(scaledH));
		const float f = scaledH - static_cast<float>(sector);
		const float p = v * (1.0f - s);
		const float q = v * (1.0f - s * f);
		const float t = v * (1.0f - s * (1.0f - f));

		switch (sector % 6)
		{
		case 0: return ColorRGB<TScalar>(v, t, p);
		case 1: return ColorRGB<TScalar>(q, v, p);
		case 2: return ColorRGB<TScalar>(p, v, t);
		case 3: return ColorRGB<TScalar>(p, q, v);
		case 4: return ColorRGB<TScalar>(t, p, v);
		default: return ColorRGB<TScalar>(v, p, q);
		}
	}

	template <Scalarable TScalar>
	void ColorRGB<TScalar>::RGBToHSV(const ColorRGB<TScalar>& rgbColor, float& h, float& s, float& v) noexcept
		requires std::same_as<TScalar, f32>
	{
		const float r = rgbColor.value.x;
		const float g = rgbColor.value.y;
		const float b = rgbColor.value.z;

		const float maxV = std::max(r, std::max(g, b));
		const float minV = std::min(r, std::min(g, b));
		const float delta = maxV - minV;

		v = maxV;

		if (delta <= Mathf::Epsilon)
		{
			h = 0.0f;
			s = 0.0f;
			return;
		}

		s = (maxV <= Mathf::Epsilon) ? 0.0f : (delta / maxV);

		if (r >= maxV)
		{
			h = (g - b) / delta;
		}
		else if (g >= maxV)
		{
			h = 2.0f + (b - r) / delta;
		}
		else
		{
			h = 4.0f + (r - g) / delta;
		}

		h /= 6.0f;
		if (h < 0.0f)
		{
			h += 1.0f;
		}
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::Lerp(const ColorRGB<TScalar>& a, const ColorRGB<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <Scalarable TScalar>
	ColorRGB<TScalar> ColorRGB<TScalar>::LerpUnclamped(const ColorRGB<TScalar>& a, const ColorRGB<TScalar>& b, f32 t) noexcept
		requires std::same_as<TScalar, f32>
	{
		return ColorRGB<TScalar>(
			Mathf::Lerp(a.value.x, b.value.x, t),
			Mathf::Lerp(a.value.y, b.value.y, t),
			Mathf::Lerp(a.value.z, b.value.z, t));
	}

	template <Scalarable TScalar>
	nlohmann::json ColorRGB<TScalar>::Serialize() const
	{
		const TScalar components[]{ GetR(), GetG(), GetB() };

		if (!IsFinite())
		{
			return nlohmann::json::value_t::discarded;
		}

		return nlohmann::json(components);
	}

	template <Scalarable TScalar>
	bool ColorRGB<TScalar>::Deserialize(const nlohmann::json& value) noexcept
	{
		constexpr usize COMPONENT_COUNT = 3;
		if (!value.is_array() || value.size() != COMPONENT_COUNT)
		{
			return false;
		}

		TScalar components[COMPONENT_COUNT]{};
		for (usize index = 0; index < COMPONENT_COUNT; ++index)
		{
			const auto& component = value[index];
			const bool isNumber = std::same_as<TScalar, i32> ? component.is_number_integer() : component.is_number();
			if (!isNumber || !Mathf::TryConvertScalar(component.get<f64>(), components[index]))
			{
				return false;
			}
		}

		*this = ColorRGB<TScalar>{ components[0], components[1], components[2] };
		return true;
	}

	template class ColorRGB<i32>;
	template class ColorRGB<f32>;
}
