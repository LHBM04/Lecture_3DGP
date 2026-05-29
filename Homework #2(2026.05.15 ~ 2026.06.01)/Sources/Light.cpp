#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"

void Light::OnPreRender()
{
	if (!IsActive())
	{
		return;
	}

	GetOwner()->GetScene()->AddLight(this);
}

void Light::OnPostRender()
{
	if (!IsActive())
	{
		return;
	}

	GetOwner()->GetScene()->RemoveLight(this);
}
