#include "Precompiled.h"
#include "Service.h"

#include "Engine.h"

void Service::NotifyAdd(Engine* engine_)
{
	engine = engine_;
	OnAdd();
}

void Service::NotifyRemove()
{
	OnRemove();
	engine = nullptr;
}

Engine& Service::GetEngine() const noexcept
{
	return *engine;
}
