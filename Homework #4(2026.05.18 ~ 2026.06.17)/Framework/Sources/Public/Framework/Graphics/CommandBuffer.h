#pragma once

namespace TUK::Framework
{
	class CommandBuffer
	{
	public:
		CommandBuffer() noexcept = default;
		virtual ~CommandBuffer() noexcept = default;

		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer& operator=(const CommandBuffer&) = delete;

		CommandBuffer(CommandBuffer&&) = delete;
		CommandBuffer& operator=(CommandBuffer&&) = delete;


	};
}
