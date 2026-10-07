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
		: value(TValue{0}, TValue{0}, TValue{0})
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(TValue value_) noexcept
		: value(value_, value_, value_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(TValue r_, TValue g_, TValue b_) noexcept
		: value(r_, g_, b_)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const ColorRGB<TValue>& color_) noexcept
		: value(color_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(ColorRGB<TValue>&& color_) noexcept
		: value(color_.value)
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const Vector3D<TValue>& vector_) noexcept
		: value(vector_.GetX(), vector_.GetY(), vector_.GetZ())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const Vector4D<TValue>& vector_) noexcept
		: value(vector_.GetX(), vector_.GetY(), vector_.GetZ())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>::ColorRGB(const ColorRGBA<TValue>& color_) noexcept
		: value(color_.GetR(), color_.GetG(), color_.GetB())
	{
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(const ColorRGB<TValue>& other_) noexcept
	{
		value.x = other_.value.x;
		value.y = other_.value.y;
		value.z = other_.value.z;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue>& ColorRGB<TValue>::operator=(ColorRGB<TValue>&& other_) noexcept
	{
		value.x = other_.value.x;
		value.y = other_.value.y;
		value.z = other_.value.z;
		return *this;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::operator==(const ColorRGB<TValue>& other_) const noexcept
	{
		if constexpr (std::same_as<TValue, int>)
		{
			return value.x == other_.value.x && value.y == other_.value.y && value.z == other_.value.z;
		}
		else
		{
			return IsApproximately(*this, other_);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	bool ColorRGB<TValue>::operator!=(const ColorRGB<TValue>& other_) const noexcept
	{
		return !(*this == other_);
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
	Vector4D<TValue> ColorRGB<TValue>::ToVector4D(TValue alpha_) const noexcept
	{
		return Vector4D<TValue>(value.x, value.y, value.z, alpha_);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGBA<TValue> ColorRGB<TValue>::ToColorRGBA(TValue alpha_) const noexcept
	{
		return ColorRGBA<TValue>(*this, alpha_);
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
	bool ColorRGB<TValue>::IsApproximately(const ColorRGB<TValue>& lhs_, const ColorRGB<TValue>& rhs_, float epsilon_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::abs(lhs_.value.x - rhs_.value.x) <= epsilon_
		&& std::abs(lhs_.value.y - rhs_.value.y) <= epsilon_
		&& std::abs(lhs_.value.z - rhs_.value.z) <= epsilon_;
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float ColorRGB<TValue>::LinearToGammaSpace(float value_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value_), 1.0f / 2.2f);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	float ColorRGB<TValue>::GammaToLinearSpace(float value_) noexcept
		requires std::same_as<TValue, float>
	{
		return std::pow(std::max(0.0f, value_), 2.2f);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::HSVToRGB(float h_, float s_, float v_) noexcept
		requires std::same_as<TValue, float>
	{
		return HSVToRGB(h_, s_, v_, false);
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::HSVToRGB(float h_, float s_, float v_, bool hdr_) noexcept
		requires std::same_as<TValue, float>
	{
		h_ = h_ - std::floor(h_);
		s_ = std::clamp(s_, 0.0f, 1.0f);
		if (!hdr_)
		{
			v_ = std::clamp(v_, 0.0f, 1.0f);
		}

		if (s_ <= std::numeric_limits<float>::epsilon())
		{
			return ColorRGB<TValue>(v_, v_, v_);
		}

		const float scaledH{ h_ * 6.0f };
		const int sector{ static_cast<int>(std::floor(scaledH)) };
		const float f{ scaledH - static_cast<float>(sector) };
		const float p{ v_ * (1.0f - s_) };
		const float q{ v_ * (1.0f - s_ * f) };
		const float t{ v_ * (1.0f - s_ * (1.0f - f)) };

		switch (sector % 6)
		{
		case 0: return ColorRGB<TValue>(v_, t, p);
		case 1: return ColorRGB<TValue>(q, v_, p);
		case 2: return ColorRGB<TValue>(p, v_, t);
		case 3: return ColorRGB<TValue>(p, q, v_);
		case 4: return ColorRGB<TValue>(t, p, v_);
		default: return ColorRGB<TValue>(v_, p, q);
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	void ColorRGB<TValue>::RGBToHSV(const ColorRGB<TValue>& rgbColor_, float& h_, float& s_, float& v_) noexcept
		requires std::same_as<TValue, float>
	{
		const float r{ rgbColor_.value.x };
		const float g{ rgbColor_.value.y };
		const float b{ rgbColor_.value.z };

		const float maxV{ std::max(r, std::max(g, b)) };
		const float minV{ std::min(r, std::min(g, b)) };
		const float delta{ maxV - minV };

		v_ = maxV;

		if (delta <= std::numeric_limits<float>::epsilon())
		{
			h_ = 0.0f;
			s_ = 0.0f;
			return;
		}

		s_ = (maxV <= std::numeric_limits<float>::epsilon()) ? 0.0f : (delta / maxV);

		if (r >= maxV)
		{
			h_ = (g - b) / delta;
		}
		else if (g >= maxV)
		{
			h_ = 2.0f + (b - r) / delta;
		}
		else
		{
			h_ = 4.0f + (r - g) / delta;
		}

		h_ /= 6.0f;
		if (h_ < 0.0f)
		{
			h_ += 1.0f;
		}
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::Lerp(const ColorRGB<TValue>& a_, const ColorRGB<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return LerpUnclamped(a_, b_, std::clamp(t_, 0.0f, 1.0f));
	}

	template <class TValue>
		requires (std::same_as<TValue, int> || std::same_as<TValue, float>)
	ColorRGB<TValue> ColorRGB<TValue>::LerpUnclamped(const ColorRGB<TValue>& a_, const ColorRGB<TValue>& b_, float t_) noexcept
		requires std::same_as<TValue, float>
	{
		return ColorRGB<TValue>(
			ScalarLerp(a_.value.x, b_.value.x, t_),
			ScalarLerp(a_.value.y, b_.value.y, t_),
			ScalarLerp(a_.value.z, b_.value.z, t_));
	}

	template class ColorRGB<int>;
	template class ColorRGB<float>;
}
