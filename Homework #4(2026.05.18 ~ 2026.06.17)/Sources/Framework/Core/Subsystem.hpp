#pragma once

#include <cstdint>

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
		[[nodiscard]] std::uint16_t GetPriority() const noexcept;

	protected:
		explicit Subsystem(std::uint16_t priority) noexcept;

		virtual SystemStatus OnStartup();
		virtual void OnShutdown();

	private:
		std::uint16_t priority;
	};
}
