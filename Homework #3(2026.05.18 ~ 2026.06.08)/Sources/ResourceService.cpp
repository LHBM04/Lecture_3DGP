#include "Precompiled.h"
#include "ResourceService.h"

void ResourceService::OnAdd()
{
}

void ResourceService::OnRemove()
{
	loaders.clear();
}
