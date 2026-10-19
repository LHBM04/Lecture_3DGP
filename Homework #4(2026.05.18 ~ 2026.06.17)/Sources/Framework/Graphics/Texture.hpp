#pragma once

#include <cassert>
#include <d3d12.h>
#include <wrl.h>

namespace TUK::Framework
{
	class CommandBuffer;
	class RenderSubsystem;
	class RenderTarget;

	class Texture
	{
		friend class CommandBuffer;
		friend class RenderSubsystem;
		friend class RenderTarget;

	public:
		virtual ~Texture() noexcept = default;

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&&) noexcept = default;
		Texture& operator=(Texture&&) noexcept = default;

	protected:
		Texture() noexcept = default;
	};
}
