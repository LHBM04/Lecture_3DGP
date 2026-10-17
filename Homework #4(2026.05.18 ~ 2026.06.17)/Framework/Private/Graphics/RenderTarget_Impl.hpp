#pragma once

#include "Framework/Graphics/RenderTarget.hpp"

namespace TUK::Framework
{
	class RenderTarget_Impl : public RenderTarget
	{
	public:
		RenderTarget_Impl() noexcept = default;
		~RenderTarget_Impl() noexcept override = default;

	private:
	};
}