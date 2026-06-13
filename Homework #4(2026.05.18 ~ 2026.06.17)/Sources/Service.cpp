#include "Precompiled.h"
#include "Service.h"

void Service::NotifyAdd(Framework* framework_)
{
	// TODO: assert()로 framework_ == nullptr 상황 방지.
	framework = framework_;
	OnAdd();
}

void Service::NotifyRemove()
{
	OnRemove();
	framework = nullptr;
}

Framework* Service::GetFramework() const noexcept
{
	return framework;
}
