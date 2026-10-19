#pragma once

#include "Buffer.hpp"

namespace TUK::Framework
{
	class Mesh
	{
	public:
		virtual ~Mesh() noexcept = default;

		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		
		Mesh(Mesh&&) noexcept = default;
		Mesh& operator=(Mesh&&) noexcept = default;

	private:
		Mesh() noexcept = default;
	};
}
