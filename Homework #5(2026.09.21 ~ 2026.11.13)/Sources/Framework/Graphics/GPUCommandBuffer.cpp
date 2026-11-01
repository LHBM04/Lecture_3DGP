#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include "../Math/Matrix4x4.hpp"
#include "../Math/ColorRGBA.hpp"

#include "Camera.hpp"
#include "GPUTexture.hpp"
#include "GPUBuffer.hpp"
#include "GPUDevice.hpp"

namespace TUK::Framework
{
	namespace
	{
		struct CameraConstants final
		{
			Matrix4x4<f32> view;
			Matrix4x4<f32> projection;
		};

		struct ObjectConstants final
		{
			Matrix4x4<f32> world;
		};

		struct MaterialConstants final
		{
			ColorRGBA<f32> baseColor;
			ColorRGBA<f32> emissiveColor;
			f32 metallic;
			f32 roughness;
			f32 padding[2]{};
		};

		enum class GPUConstantBufferSlot : u32
		{
			CAMERA = 0,
			OBJECT = 1,
			MATERIAL = 2,
		};
	}

	void GPUCommandBuffer::Reset()
	{
		d3dCommandAllocator->Reset();
		d3dCommandList->Reset(d3dCommandAllocator.Get(), nullptr);
	}

	void GPUCommandBuffer::Submit()
	{
		d3dCommandList->Close();

		ID3D12CommandList* commandLists[] = { d3dCommandList.Get() };
		device->GetD3DCommandQueue().ExecuteCommandLists(1, commandLists);
	}

	void GPUCommandBuffer::TransitionTexture(
		const GPUTexture& texture, 
		D3D12_RESOURCE_STATES before, 
		D3D12_RESOURCE_STATES after)
	{
		if (before == after)
		{
			return;
		}

		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(&texture.GetD3DResource(), before, after);
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void GPUCommandBuffer::TransitionBuffer(
		const GPUBuffer& buffer, 
		D3D12_RESOURCE_STATES before,
		D3D12_RESOURCE_STATES after)
	{
		if (before == after)
		{
			return;
		}

		assert(buffer.GetD3DHeapType() == D3D12_HEAP_TYPE_DEFAULT);

		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(&buffer.GetD3DResource(), before, after);
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void GPUCommandBuffer::SetCamera(const Camera& camera) noexcept
	{
		CameraConstants constants{};
		// DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<f32>::Load(camera.GetViewMatrix()));
		// DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<f32>::Load(camera.GetProjectionMatrix()));

		d3dCommandList->RSSetViewports(1, &camera.GetViewport());
		d3dCommandList->RSSetScissorRects(1, &camera.GetScissorRect());

		// const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		// d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::CAMERA), address);
	}

	void GPUCommandBuffer::SetMaterial(const MaterialAsset& material) noexcept
	{
		// const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		// d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::MATERIAL), address);
	}

	void GPUCommandBuffer::SetEntity(const Entity& entity) noexcept
	{
		
	}

	void GPUCommandBuffer::Draw(u32 vertexCount, u32 firstVertex)
	{
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(u32 indexCount, u32 firstIndex, u32 baseVertex)
	{
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}
}
