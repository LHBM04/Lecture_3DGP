#include "Precompiled.h"
#include "Service.h"

void Service::NotifyAdd(Framework* framework_)
{
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
