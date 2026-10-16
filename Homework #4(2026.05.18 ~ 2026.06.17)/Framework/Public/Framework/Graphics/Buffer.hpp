#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>

#include <d3d12.h>

#include <wrl.h>

namespace TUK::Framework
{
	class RenderSubsystem;
	class CommandBuffer;

	class Buffer
	{
		friend class RenderSubsystem;
		friend class CommandBuffer;

	public:
		virtual ~Buffer() noexcept = default;

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		Buffer(Buffer&& other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;

	protected:
		Buffer() noexcept = default;
	};
}
