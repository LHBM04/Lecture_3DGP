#pragma once

#include "Engine.h"

class EngineContext
{
public:
	EngineContext(Engine& engine_) :
		engine(engine_)
	{

	}

	template <std::derived_from<SystemBase> TSystem>
	auto* AddSystem(this auto& self_);

	template <std::derived_from<SystemBase> TSystem>
	auto* GetSystem(this auto& self_);

private:
	Engine& engine;
};

template <std::derived_from<SystemBase> TSystem>
auto* Engine::AddSystem(this auto& self_)
{
	return self_.engine->AddSystem<TSystem>();
}

template <std::derived_from<SystemBase> TSystem>
auto* Engine::GetSystem(this auto& self_)
{
	return self_.engine->GetSystem<TSystem>();
}
