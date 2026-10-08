#include "Precompiled.h"
#include "ColorRGB.h"

#include "ColorRGBA.h"

namespace
{
	float ScalarLerp(float a, float b, float t) noexcept
	{
		return a + t * (b - a);
	}
}

namespace TUK::Framework
{
	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB() noexcept
		: value(static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0))
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(TValue value) noexcept
		: value(value, value, value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(TValue r, TValue g, TValue b) noexcept
		: value(r, g, b)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const ColorRGB<TValue>& color) noexcept
		: value(color.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(ColorRGB<TValue>&& color) noexcept
		: value(color.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const Vector3D<TValue>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const Vector4D<TValue>& vector) noexcept
		: value(vector.GetX(), vector.GetY(), vector.GetZ())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const ColorRGBA<TValue>& color) noexcept
		: value(color.GetR(), color.GetG(), color.GetB())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(const ColorRGB<TValue>& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(ColorRGB<TValue>&& other) noexcept
	{
		value.x = other.value.x;
		value.y = other.value.y;
		value.z = other.value.z;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::operator==(const ColorRGB<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return value.x == other.value.x && value.y == other.value.y && value.z == other.value.z;
		}
		else
		{
			return IsApproximately(*this, other);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::operator!=(const ColorRGB<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGB<TValue>::GetR() const noexcept
	{
		return value.x;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::SetR(TValue component) noexcept
	{
		value.x = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGB<TValue>::GetG() const noexcept
	{
		return value.y;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::SetG(TValue component) noexcept
	{
		value.y = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGB<TValue>::GetB() const noexcept
	{
		return value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::SetB(TValue component) noexcept
	{
		value.z = component;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::Set(TValue r, TValue g, TValue b) noexcept
	{
		value.x = r;
		value.y = g;
		value.z = b;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetGamma() const noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			LinearToGammaSpace(value.x),
			LinearToGammaSpace(value.y),
			LinearToGammaSpace(value.z));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetLinear() const noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			GammaToLinearSpace(value.x),
			GammaToLinearSpace(value.y),
			GammaToLinearSpace(value.z));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float ColorRGB<TValue>::GetGrayscale() const noexcept
		requires std::same_as<TValue, float>
	{
		return 0.299f * value.x + 0.587f * value.y + 0.114f * value.z;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	TValue ColorRGB<TValue>::GetMaxColorComponent() const noexcept
	{
		return std::max(value.x, std::max(value.y, value.z));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::IsHDR() const noexcept
	{
		constexpr TValue maximum = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1);
		return value.x > maximum || value.y > maximum || value.z > maximum;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector3D<TValue> ColorRGB<TValue>::ToVector3D() const noexcept
	{
		return Vector3D<TValue>(value.x, value.y, value.z);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	Vector4D<TValue> ColorRGB<TValue>::ToVector4D(TValue alpha) const noexcept
	{
		return Vector4D<TValue>(value.x, value.y, value.z, alpha);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGB<TValue>::ToColorRGBA(TValue alpha) const noexcept
	{
		return ColorRGBA<TValue>(*this, alpha);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetBlack() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(0, 0, 0);
		}
		else
		{
			return ColorRGB<TValue>(0.0f, 0.0f, 0.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetWhite() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(255, 255, 255);
		}
		else
		{
			return ColorRGB<TValue>(1.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetRed() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(255, 0, 0);
		}
		else
		{
			return ColorRGB<TValue>(1.0f, 0.0f, 0.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetGreen() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(0, 255, 0);
		}
		else
		{
			return ColorRGB<TValue>(0.0f, 1.0f, 0.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetBlue() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(0, 0, 255);
		}
		else
		{
			return ColorRGB<TValue>(0.0f, 0.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetYellow() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(255, 255, 0);
		}
		else
		{
			return ColorRGB<TValue>(1.0f, 1.0f, 0.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetCyan() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(0, 255, 255);
		}
		else
		{
			return ColorRGB<TValue>(0.0f, 1.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetMagenta() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(255, 0, 255);
		}
		else
		{
			return ColorRGB<TValue>(1.0f, 0.0f, 1.0f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetGray() noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return ColorRGB<TValue>(128, 128, 128);
		}
		else
		{
			return ColorRGB<TValue>(0.5f, 0.5f, 0.5f);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::GetGrey() noexcept
	{
		return GetGray();
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	DirectX::XMVECTOR ColorRGB<TValue>::Load(const ColorRGB<TValue>& color) noexcept
		requires std::same_as<TValue, float>
	{
		return DirectX::XMLoadFloat3(&color.value);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::Store(ColorRGB<TValue>& destination, DirectX::XMVECTOR source) noexcept
		requires std::same_as<TValue, float>
	{
		DirectX::XMStoreFloat3(&destination.value, source);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::IsApproximately(const ColorRGB<TValue>& lhs, const ColorRGB<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.value.x - rhs.value.x) <= epsilon
		&& std::abs(lhs.value.y - rhs.value.y) <= epsilon
		&& std::abs(lhs.value.z - rhs.value.z) <= epsilon;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float ColorRGB<TValue>::LinearToGammaSpace(float value) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value), 1.0f / 2.2f);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float ColorRGB<TValue>::GammaToLinearSpace(float value) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value), 2.2f);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::HSVToRGB(float h, float s, float v) noexcept
		requires std::same_as<TValue, float>
	{
		return HSVToRGB(h, s, v, false);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::HSVToRGB(float h, float s, float v, bool hdr) noexcept
		requires std::same_as<TValue, float>
	{
		h = h - std::floor(h);
		s = std::clamp(s, 0.0f, 1.0f);
		if (!hdr)
		{
			v = std::clamp(v, 0.0f, 1.0f);
		}

		if (s <= std::numeric_limits<float>::epsilon())
		{
			return ColorRGB<TValue>(v, v, v);
		}

		const float scaledH = h * 6.0f;
		const int sector = static_cast<int>(std::floor(scaledH));
		const float f = scaledH - static_cast<float>(sector);
		const float p = v * (1.0f - s);
		const float q = v * (1.0f - s * f);
		const float t = v * (1.0f - s * (1.0f - f));

		switch (sector % 6)
		{
		case 0: return ColorRGB<TValue>(v, t, p);
		case 1: return ColorRGB<TValue>(q, v, p);
		case 2: return ColorRGB<TValue>(p, v, t);
		case 3: return ColorRGB<TValue>(p, q, v);
		case 4: return ColorRGB<TValue>(t, p, v);
		default: return ColorRGB<TValue>(v, p, q);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::RGBToHSV(const ColorRGB<TValue>& rgbColor, float& h, float& s, float& v) noexcept
		requires std::same_as<TValue, float>
	{
		const float r = rgbColor.value.x;
		const float g = rgbColor.value.y;
		const float b = rgbColor.value.z;

		const float maxV = std::max(r, std::max(g, b));
		const float minV = std::min(r, std::min(g, b));
		const float delta = maxV - minV;

		v = maxV;

		if (delta <= std::numeric_limits<float>::epsilon())
		{
			h = 0.0f;
			s = 0.0f;
			return;
		}

		s = (maxV <= std::numeric_limits<float>::epsilon()) ? 0.0f : (delta / maxV);

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

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::Lerp(const ColorRGB<TValue>& a, const ColorRGB<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::LerpUnclamped(const ColorRGB<TValue>& a, const ColorRGB<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			ScalarLerp(a.value.x, b.value.x, t),
			ScalarLerp(a.value.y, b.value.y, t),
			ScalarLerp(a.value.z, b.value.z, t));
	}

	template class ColorRGB<int>;
	template class ColorRGB<float>;
}
