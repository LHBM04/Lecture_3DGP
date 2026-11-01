#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <utility>

#include "../Resources/MaterialAsset.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"

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
			float metallic;
			float roughness;
			float padding[2]{};
		};

		enum class GPUConstantBufferSlot : std::uint32_t
		{
			CAMERA = 0,
			OBJECT = 1,
			MATERIAL = 2,
			COUNT = 3
		};
	}

	std::expected<void, std::string> GPUCommandBuffer::Reset()
	{
		if (FAILED(d3dCommandAllocator->Reset()))
		{
			return std::unexpected("D3D12 Command Allocator Reset에 실패했습니다.");
		}
		if (FAILED(d3dCommandList->Reset(d3dCommandAllocator.Get(), nullptr)))
		{
			return std::unexpected("D3D12 Command List Reset에 실패했습니다.");
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
		return SetObjectTransform(transform ? transform->GetWorldMatrix() : Matrix4x4<float>::GetIdentity());
	}

	std::expected<void, std::string> GPUCommandBuffer::SetCamera(const Camera& camera) noexcept
	{
		cameraPrepared = false;
		if (activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("카메라 적용 전에 렌더 타깃을 지정해야 합니다.");
		}

		CameraConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<float>::Load(camera.GetViewMatrix()));
		DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<float>::Load(camera.GetProjectionMatrix()));
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
			return std::unexpected("머터리얼 적용 전에 렌더 타깃을 지정해야 합니다.");
		}

		if (material.GetTargetFormat() != activeTargetFormat)
		{
			return std::unexpected("머터리얼의 PSO와 렌더 타깃 포맷이 일치하지 않습니다.");
		}
		const ColorRGBA<float>& baseColor = material.GetBaseColor();
		const ColorRGBA<float>& emissiveColor = material.GetEmissiveColor();
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

	std::expected<void, std::string> GPUCommandBuffer::SetObjectTransform(const Matrix4x4<float>& worldMatrix) noexcept
	{
		if (!cameraPrepared || activeTargetFormat == DXGI_FORMAT_UNKNOWN)
		{
			return std::unexpected("오브젝트 변환 적용 전에 카메라를 지정해야 합니다.");
		}

		ObjectConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.world, Matrix4x4<float>::Load(worldMatrix));
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
		std::uint64_t destinationOffset,
		const GPUBuffer& source,
		std::uint64_t sourceOffset,
		std::uint64_t sizeInBytes)
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

	void GPUCommandBuffer::SetGPUCameraConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset) noexcept
	{
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::CAMERA), address);
	}

	void GPUCommandBuffer::SetGPUObjectConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset) noexcept
	{
		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPUConstantBufferSlot::OBJECT), address);
	}

	void GPUCommandBuffer::SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, std::uint64_t offset) noexcept
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

	void GPUCommandBuffer::Draw(std::uint32_t vertexCount, std::uint32_t firstVertex)
	{
		d3dCommandList->DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(std::uint32_t indexCount, std::uint32_t firstIndex, std::int32_t baseVertex)
	{
		d3dCommandList->DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}

	void GPUCommandBuffer::ClearGPUTarget(const GPUTarget& target, const ColorRGBA<float>& clearColor)
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

	ID3D12GraphicsCommandList10& GPUCommandBuffer::GetD3DCommandList() const noexcept
	{
		return *(d3dCommandList.Get());
	}

	ID3D12CommandAllocator& GPUCommandBuffer::GetD3DCommandAllocator() const noexcept
	{
		return *(d3dCommandAllocator.Get());
	}

	GPUCommandBuffer::GPUCommandBuffer(const GPUDevice& device, const GPUCommandBufferProperties& properties) noexcept
		: device(&device)
		, d3dCommandList(properties.d3dCommandList)
		, d3dCommandAllocator(properties.d3dCommandAllocator)
	{
	}
	std::expected<std::pair<const GPUBuffer*, std::uint64_t>, std::string> GPUCommandBuffer::UploadConstants(
		std::span<const std::byte> bytes)
	{
		constexpr std::uint64_t CONSTANT_BUFFER_PAGE_SIZE = 64 * 1024;
		constexpr std::uint64_t ALIGNMENT = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		if (!device || bytes.empty() || bytes.size() > CONSTANT_BUFFER_PAGE_SIZE)
		{
			return std::unexpected("상수 버퍼 데이터 또는 GPUDevice가 유효하지 않습니다.");
		}
		const std::uint64_t alignedSize = (bytes.size() + ALIGNMENT - 1) / ALIGNMENT * ALIGNMENT;
		if (constantBufferOffset + alignedSize > CONSTANT_BUFFER_PAGE_SIZE)
		{
			++constantBufferPageIndex;
			constantBufferOffset = 0;
		}

		if (constantBufferPageIndex == constantBufferPages.size())
		{
			const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
			const auto description = CD3DX12_RESOURCE_DESC::Buffer(CONSTANT_BUFFER_PAGE_SIZE);
			auto page = device->CreateGPUBuffer(heapProperties, description, D3D12_RESOURCE_STATE_GENERIC_READ);
			if (!page)
			{
				return std::unexpected(page.error());
			}
			constantBufferPages.emplace_back(std::move(*page));
		}

		const std::uint64_t offset = constantBufferOffset;
		GPUBuffer& page = constantBufferPages[constantBufferPageIndex];
		if (auto result = page.Write(std::span<const u8>(reinterpret_cast<const u8*>(bytes.data()), bytes.size()), offset); !result)
		{
			return std::unexpected(result.error());
		}
		constantBufferOffset += alignedSize;
		return std::pair<const GPUBuffer*, std::uint64_t>{ &page, offset };
	}


}
