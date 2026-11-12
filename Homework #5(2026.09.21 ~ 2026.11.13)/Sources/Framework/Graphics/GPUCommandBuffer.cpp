#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include "../Core/Common.hpp"

#include "GPUBuffer.hpp"
#include "GPUDevice.hpp"
#include "GPUGraphicsPipelineState.hpp"
#include "GPUTarget.hpp"
#include "GPUTexture2D.hpp"

namespace TUK::Framework
{
	namespace
	{
		constexpr u32 TEXTURE_ROOT_SLOT = 3;
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
		textureDescriptorCount = 0;
		activeTargetFormat = DXGI_FORMAT_UNKNOWN;
		activeRTV = {};
		activeDSV = {};
		activeTargetBounds = {};
		activeViewport = {};
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

	std::expected<void, std::string> GPUCommandBuffer::SetGraphicsConstants(
		u32 rootSlot, std::span<const std::byte> bytes)
	{
		auto allocation = UploadConstants(bytes);
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}

		const D3D12_GPU_VIRTUAL_ADDRESS address = allocation->first->GetGPUVirtualAddress() + allocation->second;
		d3dCommandList->SetGraphicsRootConstantBufferView(rootSlot, address);
		return {};
	}

	void GPUCommandBuffer::SetGraphicsRootConstants(
		u32 rootSlot, std::span<const std::byte> bytes, u32 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		ASSERT_FMT(rootSlot == ROOT_CONSTANT_SLOT, "루트 상수 슬롯이 아닙니다.");
		ASSERT_FMT(bytes.size() % sizeof(u32) == 0, "루트 상수 데이터는 32비트 단위여야 합니다.");
		ASSERT_FMT(offset <= ROOT_CONSTANT_COUNT, "루트 상수 오프셋이 범위를 초과합니다.");
		ASSERT_FMT(bytes.size() / sizeof(u32) <= ROOT_CONSTANT_COUNT - offset, "루트 상수 데이터가 범위를 초과합니다.");

		d3dCommandList->SetGraphicsRoot32BitConstants(
			rootSlot, static_cast<u32>(bytes.size() / sizeof(u32)), bytes.data(), offset);
	}

	void GPUCommandBuffer::Transition(
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

	void GPUCommandBuffer::Transition(
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

	void GPUCommandBuffer::SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY primitive) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->IASetPrimitiveTopology(primitive);
	}

	void GPUCommandBuffer::SetViewport(const D3D12_VIEWPORT& viewport) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		activeViewport = viewport;
		d3dCommandList->RSSetViewports(1, &viewport);
	}

	void GPUCommandBuffer::SetScissorRect(const D3D12_RECT& scissorRect) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->RSSetScissorRects(1, &scissorRect);
	}

	void GPUCommandBuffer::SetVertexBuffer(u32 slot, const D3D12_VERTEX_BUFFER_VIEW& view) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->IASetVertexBuffers(slot, 1, &view);
	}

	void GPUCommandBuffer::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& view) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->IASetIndexBuffer(&view);
	}

	void GPUCommandBuffer::Draw(u32 vertexCount, u32 firstVertex)
	{
		DrawInstanced(vertexCount, 1, firstVertex, 0);
	}

	void GPUCommandBuffer::DrawIndexed(u32 indexCount, u32 firstIndex, i32 baseVertex)
	{
		DrawIndexedInstanced(indexCount, 1, firstIndex, baseVertex, 0);
	}

	void GPUCommandBuffer::DrawInstanced(
		u32 vertexCountPerInstance,
		u32 instanceCount,
		u32 firstVertex,
		u32 firstInstance)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->DrawInstanced(vertexCountPerInstance, instanceCount, firstVertex, firstInstance);
	}

	void GPUCommandBuffer::DrawIndexedInstanced(
		u32 indexCountPerInstance,
		u32 instanceCount,
		u32 firstIndex,
		i32 baseVertex,
		u32 firstInstance)
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");
		d3dCommandList->DrawIndexedInstanced(
			indexCountPerInstance, instanceCount, firstIndex, baseVertex, firstInstance);
	}

	void GPUCommandBuffer::ClearRenderTarget(const f32 (&color)[4], const D3D12_RECT& rect) noexcept
	{
		ASSERT_FMT(activeRTV.ptr, "렌더 타깃이 설정되지 않았습니다.");
		d3dCommandList->ClearRenderTargetView(activeRTV, color, 1, &rect);
	}

	void GPUCommandBuffer::ClearDepthStencil(f32 depth, const D3D12_RECT& rect) noexcept
	{
		ASSERT_FMT(activeDSV.ptr, "깊이 버퍼가 설정되지 않았습니다.");
		d3dCommandList->ClearDepthStencilView(activeDSV, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 1, &rect);
	}

	void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const auto description = target.GetTexture().GetD3DResource().GetDesc();
		activeTargetFormat = description.Format;
		activeRTV = target.GetD3DRTVHandle();
		activeDSV = target.GetD3DDSVHandle();
		activeTargetBounds = { 0, 0, static_cast<LONG>(description.Width), static_cast<LONG>(description.Height) };
		activeViewport = {};
		d3dCommandList->OMSetRenderTargets(1, &activeRTV, FALSE, &activeDSV);
	}

	std::expected<void, std::string> GPUCommandBuffer::BindTextures(
		std::span<const GPUTexture2D* const, TEXTURE_SLOT_COUNT> textures,
		const D3D12_SHADER_RESOURCE_VIEW_DESC* firstView)
	{
		auto& d3dDevice = device->GetD3DDevice();

		constexpr u32 DESCRIPTORS_PER_PAGE = 256;
		constexpr auto HEAP_TYPE = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

		const usize pageIndex = textureDescriptorCount / DESCRIPTORS_PER_PAGE;
		const usize slot = textureDescriptorCount % DESCRIPTORS_PER_PAGE;

		if (pageIndex == textureDescriptorPages.size())
		{
			D3D12_DESCRIPTOR_HEAP_DESC description{};
			description.Type = HEAP_TYPE;
			description.NumDescriptors = DESCRIPTORS_PER_PAGE;
			description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
			if (FAILED(d3dDevice.CreateDescriptorHeap(&description, IID_PPV_ARGS(heap.GetAddressOf()))))
			{
				return std::unexpected("텍스처 디스크립터 힙 생성에 실패했습니다.");
			}
			textureDescriptorPages.push_back(std::move(heap));
		}

		ID3D12DescriptorHeap* heap = textureDescriptorPages[pageIndex].Get();
		const UINT descriptorStride = d3dDevice.GetDescriptorHandleIncrementSize(HEAP_TYPE);
		const usize offset = slot * descriptorStride;

		auto cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += offset;

		D3D12_SHADER_RESOURCE_VIEW_DESC nullView{};
		nullView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		nullView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		nullView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		nullView.Texture2D.MipLevels = 1;

		for (usize index = 0; index < textures.size(); ++index)
		{
			const GPUTexture2D* texture = textures[index];
			const D3D12_SHADER_RESOURCE_VIEW_DESC* view = texture ? nullptr : &nullView;
			if (index == 0 && firstView)
			{
				view = firstView;
			}

			d3dDevice.CreateShaderResourceView(texture ? &texture->GetD3DResource() : nullptr, view, cpuHandle);
			cpuHandle.ptr += descriptorStride;
		}

		if (slot == 0)
		{
			d3dCommandList->SetDescriptorHeaps(1, &heap);
		}

		auto gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
		gpuHandle.ptr += offset;

		d3dCommandList->SetGraphicsRootDescriptorTable(TEXTURE_ROOT_SLOT, gpuHandle);
		textureDescriptorCount += TEXTURE_SLOT_COUNT;

		return {};
	}

	std::expected<std::pair<const GPUBuffer*, u64>, std::string> GPUCommandBuffer::UploadConstants(
		std::span<const std::byte> bytes)
	{
		ASSERT_FMT(device, "GPU 디바이스가 nullptr입니다.");
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		constexpr u64 CONSTANT_BUFFER_PAGE_SIZE = 64 * 1024;
		constexpr u64 ALIGNMENT = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;

		if (bytes.empty() || bytes.size() > CONSTANT_BUFFER_PAGE_SIZE)
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
