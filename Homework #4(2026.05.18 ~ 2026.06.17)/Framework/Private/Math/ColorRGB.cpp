#include "Precompiled.hpp"
#include "ColorRGB.hpp"

#include "ColorRGBA.hpp"
#include "ColorRGB_Impl.hpp"

namespace TUK::Framework
{
	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB() noexcept
		: values{static_cast<TValue>(0), static_cast<TValue>(0), static_cast<TValue>(0)}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(TValue value) noexcept
		: values{value, value, value}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(TValue r, TValue g, TValue b) noexcept
		: values{r, g, b}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(const ColorRGB<TValue>& color) noexcept
		: values{}
	{
		std::copy_n(color.values, 3, values);
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(ColorRGB<TValue>&& color) noexcept
		: values{}
	{
		std::copy_n(color.values, 3, values);
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(const Vector3D<TValue>& vector) noexcept
		: values{vector.GetX(), vector.GetY(), vector.GetZ()}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(const Vector4D<TValue>& vector) noexcept
		: values{vector.GetX(), vector.GetY(), vector.GetZ()}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>::ColorRGB(const ColorRGBA<TValue>& color) noexcept
		: values{color.GetR(), color.GetG(), color.GetB()}
	{
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(const ColorRGB<TValue>& other) noexcept
	{
		values[0] = other.values[0];
		values[1] = other.values[1];
		values[2] = other.values[2];
		return *this;
	}

	template <IsNumeric TValue>
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(ColorRGB<TValue>&& other) noexcept
	{
		values[0] = other.values[0];
		values[1] = other.values[1];
		values[2] = other.values[2];
		return *this;
	}

	template <IsNumeric TValue>
	bool ColorRGB<TValue>::operator==(const ColorRGB<TValue>& other) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return values[0] == other.values[0] && values[1] == other.values[1] && values[2] == other.values[2];
		}
		else
		{
			return IsApproximately(*this, other);
		}
	}

	template <IsNumeric TValue>
	bool ColorRGB<TValue>::operator!=(const ColorRGB<TValue>& other) const noexcept
	{
		return !(*this == other);
	}

	template <IsNumeric TValue>
	TValue ColorRGB<TValue>::GetR() const noexcept
	{
		return values[0];
	}

	template <IsNumeric TValue>
	void ColorRGB<TValue>::SetR(TValue component) noexcept
	{
		values[0] = component;
	}

	template <IsNumeric TValue>
	TValue ColorRGB<TValue>::GetG() const noexcept
	{
		return values[1];
	}

	template <IsNumeric TValue>
	void ColorRGB<TValue>::SetG(TValue component) noexcept
	{
		values[1] = component;
	}

	template <IsNumeric TValue>
	TValue ColorRGB<TValue>::GetB() const noexcept
	{
		return values[2];
	}

	template <IsNumeric TValue>
	void ColorRGB<TValue>::SetB(TValue component) noexcept
	{
		values[2] = component;
	}

	template <IsNumeric TValue>
	void ColorRGB<TValue>::Set(TValue r, TValue g, TValue b) noexcept
	{
		values[0] = r;
		values[1] = g;
		values[2] = b;
	}

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::GetGamma() const noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			LinearToGammaSpace(values[0]),
			LinearToGammaSpace(values[1]),
			LinearToGammaSpace(values[2]));
	}

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::GetLinear() const noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			GammaToLinearSpace(values[0]),
			GammaToLinearSpace(values[1]),
			GammaToLinearSpace(values[2]));
	}

	template <IsNumeric TValue>
	float ColorRGB<TValue>::GetGrayscale() const noexcept
		requires std::same_as<TValue, float>
	{
		return 0.299f * values[0] + 0.587f * values[1] + 0.114f * values[2];
	}

	template <IsNumeric TValue>
	TValue ColorRGB<TValue>::GetMaxColorComponent() const noexcept
	{
		return std::max(values[0], std::max(values[1], values[2]));
	}

	template <IsNumeric TValue>
	bool ColorRGB<TValue>::IsFinite() const noexcept
	{
		return std::isfinite(values[0]) && std::isfinite(values[1]) && std::isfinite(values[2]);
	}

	template <IsNumeric TValue>
	bool ColorRGB<TValue>::IsHDR() const noexcept
	{
		constexpr TValue maximum = static_cast<TValue>(std::same_as<TValue, int> ? 255 : 1);
		return values[0] > maximum || values[1] > maximum || values[2] > maximum;
	}

	template <IsNumeric TValue>
	Vector3D<TValue> ColorRGB<TValue>::ToVector3D() const noexcept
	{
		return Vector3D<TValue>(values[0], values[1], values[2]);
	}

	template <IsNumeric TValue>
	Vector4D<TValue> ColorRGB<TValue>::ToVector4D(TValue alpha) const noexcept
	{
		return Vector4D<TValue>(values[0], values[1], values[2], alpha);
	}

	template <IsNumeric TValue>
	ColorRGBA<TValue> ColorRGB<TValue>::ToColorRGBA(TValue alpha) const noexcept
	{
		return ColorRGBA<TValue>(*this, alpha);
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::GetGrey() noexcept
	{
		return GetGray();
	}

	template <IsNumeric TValue>
	bool ColorRGB<TValue>::IsApproximately(const ColorRGB<TValue>& lhs, const ColorRGB<TValue>& rhs, float epsilon) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs.values[0] - rhs.values[0]) <= epsilon
		&& std::abs(lhs.values[1] - rhs.values[1]) <= epsilon
		&& std::abs(lhs.values[2] - rhs.values[2]) <= epsilon;
	}

	template <IsNumeric TValue>
	float ColorRGB<TValue>::LinearToGammaSpace(float value) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value), 1.0f / 2.2f);
	}

	template <IsNumeric TValue>
	float ColorRGB<TValue>::GammaToLinearSpace(float value) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value), 2.2f);
	}

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::HSVToRGB(float h, float s, float v) noexcept
		requires std::same_as<TValue, float>
	{
		return HSVToRGB(h, s, v, false);
	}

	template <IsNumeric TValue>
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

	template <IsNumeric TValue>
	void ColorRGB<TValue>::RGBToHSV(const ColorRGB<TValue>& rgbColor, float& h, float& s, float& v) noexcept
		requires std::same_as<TValue, float>
	{
		const float r = rgbColor.values[0];
		const float g = rgbColor.values[1];
		const float b = rgbColor.values[2];

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

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::Lerp(const ColorRGB<TValue>& a, const ColorRGB<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a, b, std::clamp(t, 0.0f, 1.0f));
	}

	template <IsNumeric TValue>
	ColorRGB<TValue> ColorRGB<TValue>::LerpUnclamped(const ColorRGB<TValue>& a, const ColorRGB<TValue>& b, float t) noexcept
		requires std::same_as<TValue, float>
	{
		ColorRGB<TValue> result{};
		Store(result, DirectX::XMVectorLerp(Load(a), Load(b), t));
		return result;
	}

	template class ColorRGB<int>;
	template class ColorRGB<float>;
}
