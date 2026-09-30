#pragma once

namespace TUK::Framework
{
	class Renderer
	{
	public:
		Renderer() noexcept;
		~Renderer() noexcept;
		
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
	};
}
