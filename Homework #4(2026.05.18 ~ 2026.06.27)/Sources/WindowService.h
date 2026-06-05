#pragma once

#include <memory>
#include <vector>

#include "Service.h"

class Window;

class WindowService : public Service
{
public:
	WindowService() noexcept = default;
	~WindowService() noexcept override = default;

protected:
	virtual void OnAdd() override;
	virtual void OnRemove() override;

private:
	std::vector<std::unique_ptr<Window>> windows;
};
