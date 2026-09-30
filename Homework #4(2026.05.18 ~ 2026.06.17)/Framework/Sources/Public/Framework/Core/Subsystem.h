#pragma once

#include <type_traits>

namespace TUK::Framework
{
	class Subsystem
	{
	public:
		Subsystem() noexcept = default;
		virtual ~Subsystem() noexcept = default;

		Subsystem(const Subsystem&) = delete;
		Subsystem& operator=(const Subsystem&) = delete;

		Subsystem(Subsystem&&) = delete;
		Subsystem& operator=(Subsystem&&) = delete;

		virtual void OnStartup() = 0;
		virtual void OnShutdown() = 0;
	};
}
