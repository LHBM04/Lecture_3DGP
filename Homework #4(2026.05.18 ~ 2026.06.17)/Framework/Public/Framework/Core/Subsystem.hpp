#pragma once

#include "SystemStatus.hpp"

namespace TUK::Framework
{
	class Subsystem
	{
		friend class System;

	public:
		virtual ~Subsystem() noexcept;
		
		/** 복사 금지 */
		Subsystem(const Subsystem&) = delete;
		Subsystem& operator=(const Subsystem&) = delete;

		/** 이동 금지 */
		Subsystem(Subsystem&&) = delete;
		Subsystem& operator=(Subsystem&&) = delete;

		/** 우선도 */
		[[nodiscard]] unsigned short GetPriority() const noexcept;

	protected:
		explicit Subsystem(unsigned short priority) noexcept;

		virtual SystemStatus OnStartup();
		virtual void OnShutdown();

	private:
		unsigned short priority;
	};
}
