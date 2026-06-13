#pragma once

#include "Service.h"

class WindowService : public Service
{
public:
	WindowService() noexcept = default;
	~WindowService() noexcept override = default;

protected:
	void OnAdd() override;
	void OnRemove() override;
};
