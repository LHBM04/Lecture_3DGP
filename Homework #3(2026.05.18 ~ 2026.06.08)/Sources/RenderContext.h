#pragma once

#include <vector>

#include "RenderRequest.h"

class RenderContext final
{
public:
	void Submit(const RenderRequest& request_);
	[[nodiscard]] const std::vector<RenderRequest>& GetRequests() const noexcept;
	void Clear() noexcept;

private:
	std::vector<RenderRequest> requests;
};
