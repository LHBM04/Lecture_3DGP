#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <cassert>
#include <cstddef>
#include <span>
#include <utility>

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"

#include "../Resources/MaterialAsset.hpp"

#include "../Core/Common.hpp"

#include "Camera.hpp"
#include "GPUBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUMesh.hpp"
#include "GPUTarget.hpp"
#include "GPUTexture2D.hpp"

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

	std::expected<void, std::string> GPUCommandBuffer::Reset()
	{
		ASSERT_FMT(d3dCommandAllocator.Get(), "d3dCommandAllocator가 nullptr입니다.");
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		if (FAILED(d3dCommandAllocator->Reset()))
		{
			return std::unexpected("D3D12 명령 할당자 초기화에 실패했습니다.");
		}
		if (FAILED(d3dCommandList->Reset(d3dCommandAllocator.Get(), nullptr)))
		{
			return std::unexpected("D3D12 명령 리스트 초기화에 실패했습니다.");
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
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		if (FAILED(d3dCommandList->Close()))
		{
			return std::unexpected("D3D12 명령 리스트 닫기에 실패했습니다.");
		}
		ID3D12CommandList* commandLists[] = { d3dCommandList.Get() };
		device->GetD3DCommandQueue().ExecuteCommandLists(1, commandLists);
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetEntity(const Entity* entity)
	{
		ASSERT_FMT(entity, "설정할 엔티티가 nullptr입니다.");

		const Transform* transform = entity->GetComponent<Transform>();
		return SetObjectTransform(transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity());
	}

	std::expected<void, std::string> GPUCommandBuffer::SetCamera(const Camera* camera)
	{
		ASSERT_FMT(camera, "설정할 카메라가 nullptr입니다.");

		cameraPrepared = false;
		if (activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("카메라를 적용하기 전에 렌더 타깃을 설정해야 합니다.");
		}

		CameraConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<f32>::Load(camera->GetViewMatrix()));
		DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<f32>::Load(camera->GetProjectionMatrix()));
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		SetGPUCameraConstantBuffer(*allocation->first, allocation->second);
		SetViewport(camera->GetViewport());
		SetScissorRect(camera->GetScissorRect());
		cameraPrepared = true;
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetMaterial(
		const MaterialAsset* material,
		const GPUGraphicsPipelineState& pipelineState,
		DXGI_FORMAT targetFormat)
	{
		ASSERT_FMT(material, "설정할 머티리얼이 nullptr입니다.");

		if (activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("머티리얼을 적용하기 전에 렌더 타깃을 설정해야 합니다.");
		}

		if (targetFormat != activeTargetFormat)
		{
			return std::unexpected("머티리얼 파이프라인 형식이 렌더 타깃 형식과 일치하지 않습니다.");
		}
		const ColorRGBA<f32>& baseColor = material->GetBaseColor();
		const ColorRGBA<f32>& emissiveColor = material->GetEmissiveColor();
		MaterialConstants constants{};
		constants.baseColor = { baseColor.GetR(), baseColor.GetG(), baseColor.GetB(), baseColor.GetA() };
		constants.emissiveColor = { emissiveColor.GetR(), emissiveColor.GetG(), emissiveColor.GetB(), emissiveColor.GetA() };
		constants.metallic = material->GetMetallic();
		constants.roughness = material->GetRoughness();
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		SetGPUMaterialConstantBuffer(*allocation->first, allocation->second);
		SetGPUGraphicsPipelineState(pipelineState);
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::SetObjectTransform(const Matrix4x4<f32>& worldMatrix)
	{
		if (!cameraPrepared || activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("오브젝트 변환을 적용하기 전에 카메라와 렌더 타깃을 설정해야 합니다.");
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
		const GPUTexture2D& texture,
		D3D12_RESOURCE_STATES stateBefore,
		D3D12_RESOURCE_STATES stateAfter)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

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
		ASSERT_FMT(buffer.GetD3DHeapType() == D3D12_HEAP_TYPE_DEFAULT, "buffer는 DEFAULT 힙에 있어야 합니다.");
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		if (stateBefore == stateAfter)
		{
			return;
		}

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
		ASSERT_FMT(&destination.GetD3DResource() != &source.GetD3DResource(), "destination과 source가 같은 리소스를 가리킵니다.");
		ASSERT_FMT(destination.GetD3DHeapType() != D3D12_HEAP_TYPE_UPLOAD, "destination은 UPLOAD 힙에 있을 수 없습니다.");
		ASSERT_FMT(source.GetD3DHeapType() != D3D12_HEAP_TYPE_READBACK, "source는 READBACK 힙에 있을 수 없습니다.");
		ASSERT_FMT(destinationOffset <= destination.GetSizeInBytes(), "destinationOffset이 destination 크기를 초과합니다.");
		ASSERT_FMT(sizeInBytes <= destination.GetSizeInBytes() - destinationOffset, "sizeInBytes가 destination의 남은 범위를 초과합니다.");
		ASSERT_FMT(sourceOffset <= source.GetSizeInBytes(), "sourceOffset이 source 크기를 초과합니다.");
		ASSERT_FMT(sizeInBytes <= source.GetSizeInBytes() - sourceOffset, "sizeInBytes가 source의 남은 범위를 초과합니다.");

		if (sizeInBytes == 0)
		{
			return;
		}

		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		d3dCommandList->CopyBufferRegion(
			&destination.GetD3DResource(), destinationOffset,
			&source.GetD3DResource(), sourceOffset, sizeInBytes);
	}

	void GPUCommandBuffer::SetGPUGraphicsPipelineState(const GPUGraphicsPipelineState& pipelineState) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->SetPipelineState(&pipelineState.GetD3DPipelineState());
	}

	void GPUCommandBuffer::SetGPUCameraConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::CAMERA), address);
	}

	void GPUCommandBuffer::SetGPUObjectConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::OBJECT), address);
	}

	void GPUCommandBuffer::SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::MATERIAL), address);
	}

	void GPUCommandBuffer::SetViewport(const D3D12_VIEWPORT& viewport) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->RSSetViewports(1, &viewport);
	}

	void GPUCommandBuffer::SetScissorRect(const D3D12_RECT& scissorRect) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void GPUCommandBuffer::SetGPUMesh(const GPUMesh& mesh) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_VERTEX_BUFFER_VIEW vertexView = mesh.GetD3DVertexBufferView();
		const D3D12_INDEX_BUFFER_VIEW indexView = mesh.GetD3DIndexBufferView();

		d3dCommandList->IASetPrimitiveTopology(mesh.GetD3DTopology());
		d3dCommandList->IASetVertexBuffers(0, 1, &vertexView);
		d3dCommandList->IASetIndexBuffer(&indexView);
	}

	void GPUCommandBuffer::Draw(u32 vertexCount, u32 firstVertex)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(u32 indexCount, u32 firstIndex, i32 baseVertex)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}

	void GPUCommandBuffer::ClearGPUTarget(const GPUTarget& target, const ColorRGBA<f32>& clearColor)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const FLOAT color[] =
		{
			clearColor.GetR(),
			clearColor.GetG(),
			clearColor.GetB(),
			clearColor.GetA()
		};

		d3dCommandList->ClearRenderTargetView(target.GetD3DRTVHandle(), color, 0, nullptr);
		d3dCommandList->ClearDepthStencilView(
			target.GetD3DDSVHandle(),
			D3D12_CLEAR_FLAG_DEPTH,
			GPUTarget::DEPTH_CLEAR_VALUE,
			0,
			0,
			nullptr);
	}

	void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		activeTargetFormat = target.GetTexture().GetD3DResource().GetDesc().Format;
		cameraPrepared = false;
		const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = target.GetD3DRTVHandle();
		const D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = target.GetD3DDSVHandle();
		d3dCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}

	std::expected<std::pair<const GPUBuffer*, u64>, std::string> GPUCommandBuffer::UploadConstants(
		std::span<const std::byte> bytes)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		constexpr u64 CONSTANT_BUFFER_PAGE_SIZE = 64 * 1024;
		constexpr u64 ALIGNMENT = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		if (!device || bytes.empty() || bytes.size() > CONSTANT_BUFFER_PAGE_SIZE)
		{
			return std::unexpected("GPU 디바이스 또는 상수 버퍼 데이터가 유효하지 않습니다.");
		}

		const u64 alignedSize = (bytes.size() + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
		if (constantBufferOffset + alignedSize > CONSTANT_BUFFER_PAGE_SIZE)
		{
			++constantBufferPageIndex;
			constantBufferOffset = 0;
		}

		if (constantBufferPageIndex == constantBufferPages.size())
		{
			auto page = device->CreateBuffer(
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
