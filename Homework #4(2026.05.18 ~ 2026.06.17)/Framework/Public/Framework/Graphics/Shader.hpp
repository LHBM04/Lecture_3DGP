#pragma once

namespace TUK::Framework
{
	class Shader
	{
	public:
		Shader() noexcept = default;
		~Shader() noexcept = default;

		/** 복사 금지 */
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&&) noexcept = default;
		Shader& operator=(Shader&&) noexcept = default;

	private:
	};
}
