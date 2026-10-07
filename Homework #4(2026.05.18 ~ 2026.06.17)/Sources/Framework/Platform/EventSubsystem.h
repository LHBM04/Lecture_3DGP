#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "../Core/EngineSubsystem.h"

namespace TUK::Framework
{
	class EventSubsystem : public EngineSubsystem
	{
	public:
		EventSubsystem() noexcept;
		~EventSubsystem() noexcept override;

		/** WindowSubsystem이 등록할 창 클래스의 메시지 처리 함수. */
		[[nodiscard]] static WNDPROC GetWindowProc() noexcept;

	protected:
		void OnPreTick() override;
	};
}
