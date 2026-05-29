#include "Precompiled.h"
#include "Camera.h"

void Camera::OnPreRender()
{
	GetOwner()->GetScene()->AddCamera(this);
}

void Camera::OnPostRender()
{
	GetOwner()->GetScene()->RemoveCamera(this);
}