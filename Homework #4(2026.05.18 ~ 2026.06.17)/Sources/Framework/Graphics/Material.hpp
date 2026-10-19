#pragma once

#include <optional>
#include <utility>
#include "Shader.hpp"

namespace TUK::Framework
{
	class Material final
	{
	public:
		virtual ~Material() noexcept = default;

		Material(const Material&) = delete;
		Material& operator=(const Material&) = delete;

		Material(Material&&) noexcept = default;
		Material& operator=(Material&&) noexcept = default;

	private:
		Material() noexcept = default;
	};
}
