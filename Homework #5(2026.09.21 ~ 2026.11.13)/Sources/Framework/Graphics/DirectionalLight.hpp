#pragma once

#include <algorithm>

#include "../Core/Common.hpp"

#include "../Math/ColorRGB.hpp"
#include "../Math/ColorRGBA.hpp"
#include "../Math/Vector3D.hpp"

#include "../Objects/Component.hpp"

#include "GPUConstants.hpp"

namespace TUK::Framework
{
	class DirectionalLight final : public Component
	{
	public:
		using Component::Component;

		void OnEnable() override;
		void OnDisable() override;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] ColorRGB<f32> GetColorRGB() const noexcept;
		void SetColor(const ColorRGB<f32>& value) noexcept;

		[[nodiscard]] f32 GetIntensity() const noexcept;
		void SetIntensity(f32 value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetAmbientColor() const noexcept;
		void SetAmbientColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] ColorRGB<f32> GetAmbientColorRGB() const noexcept;
		void SetAmbientColor(const ColorRGB<f32>& value) noexcept;

		[[nodiscard]] Vector3D<f32> GetDirection() const noexcept;
		void SetDirection(const Vector3D<f32>& value) noexcept;

		[[nodiscard]] Vector3D<f32> GetToLightDirection() const noexcept;

		[[nodiscard]] bool IsUsingTransformRotation() const noexcept;
		void SetUseTransformRotation(bool value) noexcept;

		[[nodiscard]] ColorRGBA<f32> GetEffectiveColor() const noexcept;
		[[nodiscard]] ColorRGBA<f32> GetEffectiveAmbientColor() const noexcept;

		[[nodiscard]] DirectionalLightConstants GetConstants() const noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		ColorRGBA<f32> ambientColor{ 0.15f, 0.15f, 0.15f, 1.0f };

		Vector3D<f32> direction{ -0.5f, -1.0f, 0.5f };
		f32 intensity = 1.0f;

		bool useTransformRotation = true;
	};

	inline const ColorRGBA<f32>& DirectionalLight::GetColor() const noexcept
	{
		return color;
	}

	inline void DirectionalLight::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}

	inline ColorRGB<f32> DirectionalLight::GetColorRGB() const noexcept
	{
		return color.ToColorRGB();
	}

	inline void DirectionalLight::SetColor(const ColorRGB<f32>& value) noexcept
	{
		color = value.ToColorRGBA(1.0f);
	}

	inline f32 DirectionalLight::GetIntensity() const noexcept
	{
		return intensity;
	}

	inline void DirectionalLight::SetIntensity(f32 value) noexcept
	{
		intensity = std::max(0.0f, value);
	}

	inline const ColorRGBA<f32>& DirectionalLight::GetAmbientColor() const noexcept
	{
		return ambientColor;
	}

	inline void DirectionalLight::SetAmbientColor(const ColorRGBA<f32>& value) noexcept
	{
		ambientColor = value;
	}

	inline ColorRGB<f32> DirectionalLight::GetAmbientColorRGB() const noexcept
	{
		return ambientColor.ToColorRGB();
	}

	inline void DirectionalLight::SetAmbientColor(const ColorRGB<f32>& value) noexcept
	{
		ambientColor = value.ToColorRGBA(1.0f);
	}

	inline void DirectionalLight::SetDirection(const Vector3D<f32>& value) noexcept
	{
		direction = value;
	}

	inline Vector3D<f32> DirectionalLight::GetToLightDirection() const noexcept
	{
		return -GetDirection();
	}

	inline bool DirectionalLight::IsUsingTransformRotation() const noexcept
	{
		return useTransformRotation;
	}

	inline void DirectionalLight::SetUseTransformRotation(bool value) noexcept
	{
		useTransformRotation = value;
	}

	inline ColorRGBA<f32> DirectionalLight::GetEffectiveColor() const noexcept
	{
		return color * intensity;
	}

	inline ColorRGBA<f32> DirectionalLight::GetEffectiveAmbientColor() const noexcept
	{
		return ambientColor * intensity;
	}
}
