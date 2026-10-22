#include "Precompiled.hpp"
#include "Component.hpp"

namespace TUK::Framework
{
	Component::Component() noexcept
		: owner(nullptr)
		, isStared(false)
		, isEnabled(true)
		, isDestroyed(false)
	{
	}

	Component::~Component() noexcept
	{	
	}

	Entity* Component::GetOwner()
	{
		return owner;
	}

	const Entity* Component::GetOwner() const
	{
		return owner;
	}

	void Component::OnAwake()
	{
	}

	void Component::OnStart()
	{
	}

	void Component::OnEnable()
	{
	}

	void Component::OnDisable()
	{
	}

	void Component::OnFixedUpdate()
	{
	}

	void Component::OnUpdate()
	{
	}

	void Component::OnLateUpdate()
	{
	}

	void Component::OnPreRender(CommandBuffer&)
	{
	}

	void Component::OnRender(CommandBuffer&)
	{
	}

	void Component::OnPostRender(CommandBuffer&)
	{
	}

	void Component::OnDestroy()
	{
	}
}
