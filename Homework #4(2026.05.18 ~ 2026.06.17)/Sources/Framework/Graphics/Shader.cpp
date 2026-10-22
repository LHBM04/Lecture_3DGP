#include "Precompiled.hpp"
#include "Shader.hpp"

namespace TUK::Framework
{
	Shader::Shader(ID3DBlob* d3dShaderBlob) noexcept
		: d3dBlob(d3dShaderBlob)
	{
		assert(d3dShaderBlob);
	}

	Shader::~Shader() noexcept
	{
		if (d3dBlob)
		{
			d3dBlob->Release();
			d3dBlob = nullptr;
		}
	}

	Shader::Shader(Shader&& other) noexcept
		: d3dBlob(std::exchange(other.d3dBlob, nullptr))
	{
	}

	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			if (d3dBlob)
			{
				d3dBlob->Release();
			}
			d3dBlob = std::exchange(other.d3dBlob, nullptr);
		}
		return *this;
	}

	ID3DBlob* Shader::GetBlob() const noexcept
	{
		return d3dBlob;
	}
}
