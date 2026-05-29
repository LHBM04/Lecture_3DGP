#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"

void Light::OnPreRender()
{
	GetOwner()->GetScene()->AddLight(this);
}

void Light::OnPostRender()
{
	GetOwner()->GetScene()->RemoveLight(this);
}
