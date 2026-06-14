#include "Precompiled.h"
#include "Service.h"

void Service::NotifyAdd(Framework* framework_)
{
	assert(framework_ != nullptr);

	framework = framework_;
	OnAdd();
}

void Service::NotifyRemove()
{
	assert(framework != nullptr);

	OnRemove();
	framework = nullptr;
}

Framework* Service::GetFramework() const noexcept
{
	assert(framework != nullptr);
	return framework;
}
