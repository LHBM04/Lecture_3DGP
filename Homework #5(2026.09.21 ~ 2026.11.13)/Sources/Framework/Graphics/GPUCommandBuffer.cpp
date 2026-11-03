#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <cassert>
#include <cstddef>
#include <span>
#include <utility>

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"

#include "../Resources/MaterialAsset.hpp"

#include "Camera.hpp"
#include "GPUBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUMesh.hpp"
#include "GPUTarget.hpp"
#include "GPUTexture.hpp"

namespace TUK::Framework
{
	namespace
	{
		struct CameraConstants final
		{
			DirectX::XMFLOAT4X4 view;
			DirectX::XMFLOAT4X4 projection;
		};

		struct ObjectConstants final
		{
			DirectX::XMFLOAT4X4 world;
		};

		struct MaterialConstants final
		{
			DirectX::XMFLOAT4 baseColor;
			DirectX::XMFLOAT4 emissiveColor;
			f32 metallic;
			f32 roughness;
			f32 padding[2]{};
		};

		enum class GPUConstantBufferSlot : u32
		{
			CAMERA = 0,
			OBJECT = 1,
			MATERIAL = 2,
			COUNT = 3
		};
	}

	std::expected<GPUCommandBuffer, std::string> GPUCommandBuffer::Create(const GPUDevice& device)
	{
		ID3D12Device& d3dDevice = device.GetD3DDevice();

		ComPtr<ID3D12CommandAllocator> d3dCommandAllocator;
		if (FAILED(d3dDevice.CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(d3dCommandAllocator.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command Allocator ?앹꽦???ㅽ뙣?덉뒿?덈떎.");
		}

		ComPtr<ID3D12GraphicsCommandList10> d3dCommandList;
		if (FAILED(d3dDevice.CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			d3dCommandAllocator.Get(),
			nullptr,
			IID_PPV_ARGS(d3dCommandList.GetAddressOf()))))
		{
			return std::unexpected("D3D12 Command List ?앹꽦???ㅽ뙣?덉뒿?덈떎.");
		}

		if (FAILED(d3dCommandList->Close()))
		{
			return std::unexpected("D3D12 Command List 珥덇린 Close???ㅽ뙣?덉뒿?덈떎.");
		}

		return GPUCommandBuffer(device, std::move(d3dCommandList), std::move(d3dCommandAllocator));
	}

	std::expected<void, std::string> GPUCommandBuffer::Reset()
	{
		if (FAILED(d3dCommandAllocator->Reset()))
		{
			return std::unexpected("D3D12 Command Allocator Reset???ㅽ뙣?덉뒿?덈떎.");
		}
		if (FAILED(d3dCommandList->Reset(d3dCommandAllocator.Get(), nullptr)))
		{
			return std::unexpected("D3D12 Command List Reset???ㅽ뙣?덉뒿?덈떎.");
		}

		constantBufferPageIndex = 0;
		constantBufferOffset = 0;
		activeTargetFormat = DXGI_FORMAT_UNKNOWN;
		cameraPrepared = false;
		d3dCommandList->SetGraphicsRootSignature(&device->GetD3DRootSignature());
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::Submit()
	{
		if (FAILED(d3dCommandList->Close()))
		{
			return std::unexpected("D3D12 Command List Close failed.");
		}
		ID3D12CommandList* commandLists[] = { d3dCommandList.Get() };
		device->GetD3DCommandQueue().ExecuteCommandLists(1, commandLists);
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetEntity(const Entity& entity) noexcept
	{
		const Transform* transform = entity.GetComponent<Transform>();
		return SetObjectTransform(transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity());
	}

	std::expected<void, std::string> GPUCommandBuffer::SetCamera(const Camera& camera) noexcept
	{
		cameraPrepared = false;
		if (activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("移대찓???곸슜 ?꾩뿉 ?뚮뜑 ?源껋쓣 吏?뺥빐???⑸땲??");
		}

		CameraConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<f32>::Load(camera.GetViewMatrix()));
		DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<f32>::Load(camera.GetProjectionMatrix()));
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		SetGPUCameraConstantBuffer(*allocation->first, allocation->second);
		SetViewport(camera.GetViewport());
		SetScissorRect(camera.GetScissorRect());
		cameraPrepared = true;
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetMaterial(const MaterialAsset& material) noexcept
	{
		if (activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("癒명꽣由ъ뼹 ?곸슜 ?꾩뿉 ?뚮뜑 ?源껋쓣 吏?뺥빐???⑸땲??");
		}

		if (material.GetTargetFormat() != activeTargetFormat)
		{
			return std::unexpected("癒명꽣由ъ뼹??PSO? ?뚮뜑 ?源??щ㎎???쇱튂?섏? ?딆뒿?덈떎.");
		}
		const ColorRGBA<f32>& baseColor = material.GetBaseColor();
		const ColorRGBA<f32>& emissiveColor = material.GetEmissiveColor();
		MaterialConstants constants{};
		constants.baseColor = { baseColor.GetR(), baseColor.GetG(), baseColor.GetB(), baseColor.GetA() };
		constants.emissiveColor = { emissiveColor.GetR(), emissiveColor.GetG(), emissiveColor.GetB(), emissiveColor.GetA() };
		constants.metallic = material.GetMetallic();
		constants.roughness = material.GetRoughness();
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		SetGPUMaterialConstantBuffer(*allocation->first, allocation->second);
		SetGPUGraphicsPipelineState(material.GetGraphicsPipelineState());
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetObjectTransform(const Matrix4x4<f32>& worldMatrix) noexcept
	{
		if (!cameraPrepared || activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("?ㅻ툕?앺듃 蹂???곸슜 ?꾩뿉 移대찓?쇰? 吏?뺥빐???⑸땲??");
		}

		ObjectConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.world, Matrix4x4<f32>::Load(worldMatrix));
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		SetGPUObjectConstantBuffer(*allocation->first, allocation->second);
		return {};
	}

	void GPUCommandBuffer::TransitionTexture(
		const GPUTexture& texture,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter)
	{
		if (stateBefore == stateAfter)
		{
			return;
		}

		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(&texture.GetD3DResource(), stateBefore, stateAfter);
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void GPUCommandBuffer::TransitionBuffer(
		const GPUBuffer& buffer,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter)
	{
		if (stateBefore == stateAfter)
		{
			return;
		}

		assert(buffer.GetD3DHeapType() == D3D12_HEAP_TYPE_DEFAULT);

		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(&buffer.GetD3DResource(), stateBefore, stateAfter);
		d3dCommandList->ResourceBarrier(1, &barrier);
	}

	void GPUCommandBuffer::CopyGPUBuffer(
		const GPUBuffer& destination,
		u64 destinationOffset,
		const GPUBuffer& source,
		u64 sourceOffset,
		u64 sizeInBytes)
	{
		assert(&destination.GetD3DResource() != &source.GetD3DResource());
		assert(destination.GetD3DHeapType() != D3D12_HEAP_TYPE_UPLOAD);
		assert(source.GetD3DHeapType() != D3D12_HEAP_TYPE_READBACK);
		assert(destinationOffset <= destination.GetSizeInBytes());
		assert(sizeInBytes <= destination.GetSizeInBytes() - destinationOffset);
		assert(sourceOffset <= source.GetSizeInBytes());
		assert(sizeInBytes <= source.GetSizeInBytes() - sourceOffset);

		if (sizeInBytes == 0)
		{
			return;
		}

		d3dCommandList->CopyBufferRegion(
			&destination.GetD3DResource(), destinationOffset,
			&source.GetD3DResource(), sourceOffset, sizeInBytes);
	}

	void GPUCommandBuffer::SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState) noexcept
	{
		d3dCommandList->SetPipelineState(&pipelineState.GetD3DPipelineState());
	}

	void GPUCommandBuffer::SetGPUCameraConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::CAMERA), address);
	}

	void GPUCommandBuffer::SetGPUObjectConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::OBJECT), address);
	}

	void GPUCommandBuffer::SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::MATERIAL), address);
	}

	void GPUCommandBuffer::SetViewport(const D3D12_VIEWPORT& viewport) noexcept
	{
		d3dCommandList->RSSetViewports(1, &viewport);
	}

	void GPUCommandBuffer::SetScissorRect(const D3D12_RECT& scissorRect) noexcept
	{
		d3dCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void GPUCommandBuffer::SetGPUMesh(const GPUMesh& mesh) noexcept
	{
		const D3D12_VERTEX_BUFFER_VIEW vertexView = mesh.GetD3DVertexBufferView();
		const D3D12_INDEX_BUFFER_VIEW indexView = mesh.GetD3DIndexBufferView();

		d3dCommandList->IASetPrimitiveTopology(mesh.GetD3DTopology());
		d3dCommandList->IASetVertexBuffers(0, 1, &vertexView);
		d3dCommandList->IASetIndexBuffer(&indexView);
	}

	void GPUCommandBuffer::Draw(u32 vertexCount, u32 firstVertex)
	{
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(u32 indexCount, u32 firstIndex, i32 baseVertex)
	{
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}

	void GPUCommandBuffer::ClearGPUTarget(const GPUTarget& target, const ColorRGBA<f32>& clearColor)
	{
		const FLOAT color[] =
		{
			clearColor.GetR(),
			clearColor.GetG(),
			clearColor.GetB(),
			clearColor.GetA()
		};

		d3dCommandList->ClearRenderTargetView(target.GetD3DRTVHandle(), color, 0, nullptr);
	}

	void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target) noexcept
	{
		activeTargetFormat = target.GetTexture().GetD3DResource().GetDesc().Format;
		cameraPrepared = false;
		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = target.GetD3DRTVHandle();
		d3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	}

	std::expected<std::pair<const GPUBuffer*, u64>, std::string> GPUCommandBuffer::UploadConstants(
		std::span<const std::byte> bytes)
	{
		constexpr u64 CONSTANT_BUFFER_PAGE_SIZE = 64 * 1024;
		constexpr u64 ALIGNMENT = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		if (!device || bytes.empty() || bytes.size() > CONSTANT_BUFFER_PAGE_SIZE)
		{
			return std::unexpected("?곸닔 踰꾪띁 ?곗씠???먮뒗 GPUDevice媛 ?좏슚?섏? ?딆뒿?덈떎.");
		}

		const u64 alignedSize = (bytes.size() + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
		if (constantBufferOffset + alignedSize > CONSTANT_BUFFER_PAGE_SIZE)
		{
			++constantBufferPageIndex;
			constantBufferOffset = 0;
		}

		if (constantBufferPageIndex == constantBufferPages.size())
		{
			auto page = GPUBuffer::Create(
				*device,
				D3D12_HEAP_TYPE_UPLOAD,
				CONSTANT_BUFFER_PAGE_SIZE,
				D3D12_RESOURCE_STATE_GENERIC_READ);
			if (!page)
			{
				return std::unexpected(page.error());
			}
			constantBufferPages.emplace_back(std::move(*page));
		}

		const u64 offset = constantBufferOffset;
		GPUBuffer& page = constantBufferPages[constantBufferPageIndex];
		if (auto result = page.Write(std::span<const u8>(reinterpret_cast<const u8*>(bytes.data()), bytes.size()), offset); !result)
		{
			return std::unexpected(result.error());
		}

		constantBufferOffset += alignedSize;
		return std::pair<const GPUBuffer*, u64>{ &page, offset };
	}
}
