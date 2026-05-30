#include "Precompiled.h"
#include "Light.h"

#include "GameObject.h"
#include "Scene.h"

void Light::OnEnable()
{
	GetOwner()->GetScene()->AddLight(this);
}

void Light::OnDisable()
{
	GetOwner()->GetScene()->RemoveLight(this);
}

