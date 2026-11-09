#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>

#include <nlohmann/json.hpp>

#include "../Core/Common.hpp"

namespace TUK::Framework::JsonUtility
{
	template <Scalarable TScalar, std::size_t N>
	[[nodiscard]] nlohmann::json SerializeComponents(const TScalar (&components)[N])
	{
		if constexpr (std::same_as<TScalar, f32>)
		{
			for (const TScalar component : components)
			{
				if (!std::isfinite(component))
				{
					return nlohmann::json::value_t::discarded;
				}
			}
		}

		return nlohmann::json(components);
	}

	template <Scalarable TScalar>
	[[nodiscard]] bool DeserializeNumber(const nlohmann::json& element, TScalar& component) noexcept
	{
		if constexpr (std::same_as<TScalar, i32>)
		{
			if (!element.is_number_integer())
			{
				return false;
			}
		}
		else if (!element.is_number())
		{
			return false;
		}

		const double number = element.get<double>();
		if (!std::isfinite(number) ||
			number < std::numeric_limits<TScalar>::lowest() ||
			number > (std::numeric_limits<TScalar>::max)())
		{
			return false;
		}

		component = static_cast<TScalar>(number);
		return true;
	}

	template <Scalarable TScalar, std::size_t N>
	[[nodiscard]] bool DeserializeComponents(const nlohmann::json& value, TScalar (&components)[N]) noexcept
	{
		if (!value.is_array() || value.size() != N)
		{
			return false;
		}

		for (std::size_t index = 0; index < N; ++index)
		{
			if (!DeserializeNumber(value[index], components[index]))
			{
				return false;
			}
		}

		return true;
	}
}
