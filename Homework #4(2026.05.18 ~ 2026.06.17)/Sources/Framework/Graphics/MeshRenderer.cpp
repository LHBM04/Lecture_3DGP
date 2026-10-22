#include "Precompiled.hpp"
#include "MeshRenderer.hpp"

#include "CommandBuffer.hpp"

namespace TUK::Framework
{
	MeshRenderer::MeshRenderer() noexcept
		: mesh(nullptr)
		, material(nullptr)
	{
	}

	MeshRenderer::~MeshRenderer() noexcept
	{
	}

	Mesh& MeshRenderer::GetMesh() const noexcept
	{
		return *mesh;
	}

	void MeshRenderer::SetMesh(Mesh& value) noexcept
	{
		mesh = &value;
	}

	Material& MeshRenderer::GetMaterial() const noexcept
	{
		return *material;
	}

	void MeshRenderer::SetMaterial(Material& value) noexcept
	{
		material = &value;
	}

	void MeshRenderer::OnRender(CommandBuffer& commandBuffer)
	{
		commandBuffer.DrawInstanced(0, 0, 0, 0);
	}
}
