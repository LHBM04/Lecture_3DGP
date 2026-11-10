#include "Precompiled.hpp"
#include "GPUCommandBuffer.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <span>
#include <utility>

#include "../Core/App.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"

#include "../Resources/AssetSubsystem.hpp"
#include "../Resources/Cubemap.hpp"
#include "../Resources/MaterialAsset.hpp"
#include "../Resources/ShaderAsset.hpp"
#include "../Resources/TextureAsset.hpp"

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
			u32 textureMask;
			f32 padding{};
			DirectX::XMFLOAT4 specularColor;
		};

		static_assert(sizeof(MaterialConstants) == 64);

		struct ImageUIConstants final
		{
			DirectX::XMFLOAT2 viewportSize;
			DirectX::XMFLOAT2 padding{};
		};

		static_assert(sizeof(ImageUIConstants) == 16);

		enum class GPURootSlot : u32
		{
			CAMERA = 0,
			OBJECT = 1,
			MATERIAL = 2,
			TEXTURE = 3
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
		textureDescriptorCount = 0;
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
		const ColorRGBA<f32>& specularColor = material->GetSpecularColor();
		if (material->IsSkybox())
		{
			return std::unexpected("메시에는 일반 2D 텍스처를 사용하는 머티리얼이 필요합니다.");
		}
		MaterialConstants constants{};
		constants.baseColor = { baseColor.GetR(), baseColor.GetG(), baseColor.GetB(), baseColor.GetA() };
		constants.emissiveColor = { emissiveColor.GetR(), emissiveColor.GetG(), emissiveColor.GetB(), emissiveColor.GetA() };
		constants.metallic = material->GetMetallic();
		constants.roughness = material->GetRoughness();
		constants.specularColor = { specularColor.GetR(), specularColor.GetG(), specularColor.GetB(), specularColor.GetA() };

		static_assert(TEXTURE_SLOT_COUNT == static_cast<u32>(MaterialTextureSlot::COUNT));
		std::array<const GPUTexture2D*, TEXTURE_SLOT_COUNT> textures{};
		const MaterialTextures& materialTextures = material->GetTextures();
		for (u32 index = 0; index < TEXTURE_SLOT_COUNT; ++index)
		{
			if (const TextureAsset* texture = materialTextures[index])
			{
				textures[index] = &texture->GetTexture();
				constants.textureMask |= 1u << index;
			}
		}
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}
		if (auto result = BindTextures(textures); !result)
		{
			return result;
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
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPURootSlot::CAMERA), address);
	}

	void GPUCommandBuffer::SetGPUObjectConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPURootSlot::OBJECT), address);
	}

	void GPUCommandBuffer::SetGPUMaterialConstantBuffer(const GPUBuffer& buffer, u64 offset) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_GPU_VIRTUAL_ADDRESS address = buffer.GetGPUVirtualAddress() + offset;
		d3dCommandList->SetGraphicsRootConstantBufferView(std::to_underlying(GPURootSlot::MATERIAL), address);
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

	void GPUCommandBuffer::SetGPUMesh(const GPUMesh& mesh) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const D3D12_VERTEX_BUFFER_VIEW vertexView = mesh.GetD3DVertexBufferView();
		const D3D12_INDEX_BUFFER_VIEW indexView = mesh.GetD3DIndexBufferView();

		d3dCommandList->IASetPrimitiveTopology(mesh.GetD3DTopology());
		d3dCommandList->IASetVertexBuffers(0, 1, &vertexView);
		d3dCommandList->IASetIndexBuffer(&indexView);
	}

	std::expected<void, std::string> GPUCommandBuffer::DrawSkybox(
		const MaterialAsset* material, const Cubemap* cubemap)
	{
		ASSERT_FMT(material, "설정할 스카이박스 머티리얼이 nullptr입니다.");

		if (!cameraPrepared || activeTargetFormat != material->GetTargetFormat())
		{
			return std::unexpected("스카이박스를 그리려면 카메라와 호환되는 렌더 타깃이 필요합니다.");
		}

		if (!material->IsSkybox() || !cubemap)
		{
			return std::unexpected("스카이박스 머티리얼과 큐브맵이 필요합니다.");
		}

		const GPUTexture2D& texture = cubemap->GetTexture();
		const auto description = texture.GetD3DResource().GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC view{};
		view.Format = description.Format;
		view.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		view.TextureCube.MipLevels = description.MipLevels;
		const std::array<const GPUTexture2D*, TEXTURE_SLOT_COUNT> textures{ &texture };
		if (auto result = BindTextures(textures, &view); !result)
		{
			return result;
		}

		SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		constexpr u32 FULLSCREEN_VERTEX_COUNT = 3;
		Draw(FULLSCREEN_VERTEX_COUNT);
		return {};
	}

	std::expected<void, std::string> GPUCommandBuffer::DrawImageUI(const Entity* entity, const TextureAsset* image)
	{
		ASSERT_FMT(entity, "UI 이미지 엔티티가 nullptr입니다.");
		ASSERT_FMT(image, "UI 이미지 텍스처가 nullptr입니다.");

		if (!cameraPrepared || activeTargetFormat == DXGI_FORMAT_UNKNOWN ||
			activeViewport.Width <= 0.0f || activeViewport.Height <= 0.0f)
		{
			return std::unexpected("UI 이미지를 그리려면 카메라와 렌더 타깃이 필요합니다.");
		}

		if (!entity->GetComponent<Transform>())
		{
			return std::unexpected("UI 이미지 엔티티에 Transform 컴포넌트가 필요합니다.");
		}

		auto pipeline = imageUIPipelines.find(activeTargetFormat);
		if (pipeline == imageUIPipelines.end())
		{
			auto created = CreateImageUIPipeline();
			if (!created)
			{
				return std::unexpected(created.error());
			}
			pipeline = imageUIPipelines.try_emplace(activeTargetFormat, std::move(*created)).first;
		}

		if (auto result = SetEntity(entity); !result)
		{
			return result;
		}

		const ImageUIConstants constants{ { activeViewport.Width, activeViewport.Height } };
		auto allocation = UploadConstants(std::as_bytes(std::span(&constants, 1)));
		if (!allocation)
		{
			return std::unexpected(allocation.error());
		}

		const std::array<const GPUTexture2D*, TEXTURE_SLOT_COUNT> textures{ &image->GetTexture() };
		if (auto result = BindTextures(textures); !result)
		{
			return result;
		}

		SetGPUMaterialConstantBuffer(*allocation->first, allocation->second);
		SetGPUGraphicsPipelineState(pipeline->second);
		d3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		constexpr u32 IMAGE_VERTEX_COUNT = 4;
		Draw(IMAGE_VERTEX_COUNT);
		return {};
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

	bool GPUCommandBuffer::ClearCameraBuffers(const Camera* camera, bool clearColor)
	{
		ASSERT_FMT(camera, "설정할 카메라가 nullptr입니다.");
		ASSERT_FMT(cameraPrepared, "클리어 전에 카메라를 설정해야 합니다.");

		const D3D12_VIEWPORT& viewport = camera->GetViewport();
		const D3D12_RECT& scissor = camera->GetScissorRect();
		const f32 targetWidth = static_cast<f32>(activeTargetBounds.right);
		const f32 targetHeight = static_cast<f32>(activeTargetBounds.bottom);
		const D3D12_RECT rect{
			std::max(scissor.left, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftX, 0.0f, targetWidth)))),
			std::max(scissor.top, static_cast<LONG>(std::floor(std::clamp(viewport.TopLeftY, 0.0f, targetHeight)))),
			std::min(scissor.right, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftX + viewport.Width, 0.0f, targetWidth)))),
			std::min(scissor.bottom, static_cast<LONG>(std::ceil(std::clamp(viewport.TopLeftY + viewport.Height, 0.0f, targetHeight))))
		};
		if (rect.left >= rect.right || rect.top >= rect.bottom)
		{
			return false;
		}

		if (clearColor)
		{
			const ColorRGBA<f32>& background = camera->GetBackgroundColor();
			const FLOAT color[] = { background.GetR(), background.GetG(), background.GetB(), background.GetA() };
			d3dCommandList->ClearRenderTargetView(activeRTV, color, 1, &rect);
		}
		d3dCommandList->ClearDepthStencilView(activeDSV, D3D12_CLEAR_FLAG_DEPTH, GPUTarget::DEPTH_CLEAR_VALUE, 0, 1, &rect);
		return true;
	}

	void GPUCommandBuffer::SetGPUTarget(const GPUTarget& target) noexcept
	{
		ASSERT_FMT(d3dCommandList.Get(), "d3dCommandList가 nullptr입니다.");

		const auto description = target.GetTexture().GetD3DResource().GetDesc();
		activeTargetFormat = description.Format;
		activeRTV = target.GetD3DRTVHandle();
		activeDSV = target.GetD3DDSVHandle();
		activeTargetBounds = { 0, 0, static_cast<LONG>(description.Width), static_cast<LONG>(description.Height) };
		cameraPrepared = false;
		d3dCommandList->OMSetRenderTargets(1, &activeRTV, FALSE, &activeDSV);
	}

	std::expected<GPUGraphicsPipelineState, std::string> GPUCommandBuffer::CreateImageUIPipeline() const
	{
		AssetSubsystem* assets = App::GetInstance().GetSubsystem<AssetSubsystem>();
		if (!assets)
		{
			return std::unexpected("에셋 서브시스템을 사용할 수 없습니다.");
		}

		constexpr DWORD EXECUTABLE_PATH_CAPACITY = 32768;
		std::wstring executablePath(EXECUTABLE_PATH_CAPACITY, L'\0');
		const DWORD length = GetModuleFileNameW(nullptr, executablePath.data(), EXECUTABLE_PATH_CAPACITY);
		if (length == 0 || length >= EXECUTABLE_PATH_CAPACITY)
		{
			return std::unexpected("실행 파일의 경로를 확인하지 못했습니다.");
		}
		executablePath.resize(length);

		const auto shaderDir = std::filesystem::path(executablePath).parent_path() / "Resources" / "Shaders";
		const ShaderAsset* vertex = assets->LoadAsset<ShaderAsset>(shaderDir / "ImageUI.vert.cso");
		const ShaderAsset* pixel = assets->LoadAsset<ShaderAsset>(shaderDir / "ImageUI.frag.cso");
		if (!vertex || !pixel)
		{
			return std::unexpected("ImageUI 셰이더를 불러오지 못했습니다.");
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC description{};
		description.VS = vertex->GetShader().GetD3DBytecode();
		description.PS = pixel->GetShader().GetD3DBytecode();
		description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		description.NumRenderTargets = 1;
		description.RTVFormats[0] = activeTargetFormat;
		description.DSVFormat = GPUTarget::DEPTH_FORMAT;
		description.SampleMask = UINT_MAX;
		description.SampleDesc = { 1, 0 };
		description.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		description.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		description.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		description.DepthStencilState.DepthEnable = FALSE;
		description.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		description.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

		auto& blend = description.BlendState.RenderTarget[0];
		blend.BlendEnable = TRUE;
		blend.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blend.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blend.SrcBlendAlpha = D3D12_BLEND_ONE;
		blend.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		return device->CreateGraphicsPipelineState(description);
	}

	std::expected<void, std::string> GPUCommandBuffer::BindTextures(
		std::span<const GPUTexture2D* const, TEXTURE_SLOT_COUNT> textures,
		const D3D12_SHADER_RESOURCE_VIEW_DESC* firstView)
	{
		constexpr u32 DESCRIPTORS_PER_PAGE = 256;
		static_assert(DESCRIPTORS_PER_PAGE % TEXTURE_SLOT_COUNT == 0);
		constexpr auto HEAP_TYPE = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		const usize pageIndex = textureDescriptorCount / DESCRIPTORS_PER_PAGE;
		const usize slot = textureDescriptorCount % DESCRIPTORS_PER_PAGE;
		auto& d3dDevice = device->GetD3DDevice();
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
		d3dCommandList->SetGraphicsRootDescriptorTable(std::to_underlying(GPURootSlot::TEXTURE), gpuHandle);
		textureDescriptorCount += TEXTURE_SLOT_COUNT;
		return {};
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
