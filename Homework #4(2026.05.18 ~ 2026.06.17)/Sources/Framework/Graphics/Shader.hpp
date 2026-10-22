#pragma once

#include <d3d12.h>

namespace TUK::Framework
{
	class Shader
	{
	public:
		explicit Shader(ID3DBlob* d3dShaderBlob) noexcept;
		~Shader() noexcept;

		/** 복사 금지 */
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		Shader(Shader&&) noexcept;
		Shader& operator=(Shader&&) noexcept;

		[[nodiscard]] ID3DBlob* GetBlob() const noexcept;

	private:
		ID3DBlob* d3dBlob;
	};
}
