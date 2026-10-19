#pragma once

namespace TUK::Framework
{
	class Pipeline
	{
	public:
		virtual ~Pipeline() noexcept = default;

		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;

		Pipeline(Pipeline&&) noexcept = default;
		Pipeline& operator=(Pipeline&&) noexcept = default;

	protected:
		Pipeline() noexcept = default;
	};
}
