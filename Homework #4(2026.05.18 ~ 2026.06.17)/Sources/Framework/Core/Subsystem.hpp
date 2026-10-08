#pragma once

namespace TUK::Framework
{
	class Subsystem
	{
		friend class System;

	public:
		explicit Subsystem(unsigned short priority) noexcept;
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
		virtual void OnStartup();
		virtual void OnShutdown();

	private:
		unsigned short priority;
	};
}
