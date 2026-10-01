#pragma once

#include <expected>
#include <string>

/** 렌더링 서브 시스템 */
namespace RenderSubsystem
{
	/** 서브 시스템 가동 */
	std::expected<void, std::string> Startup();

	/** 서브 시스템 종료 */
	void Shutdown() noexcept;
}
