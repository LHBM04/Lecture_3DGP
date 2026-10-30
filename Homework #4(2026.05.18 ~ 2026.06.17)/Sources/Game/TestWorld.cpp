#include "Precompiled.hpp"
#include "TestWorld.hpp"

#include "Framework/Assets/AssetSubsystem.hpp"
#include "Framework/Assets/MaterialAsset.hpp"
#include "Framework/Assets/ModelAsset.hpp"
#include "Framework/Assets/ShaderAsset.hpp"

#include "Framework/Core/App.hpp"
#include "Framework/Core/Engine.hpp"

#include "Framework/Entities/Entity.hpp"
#include "Framework/Entities/Transform.hpp"

#include "Framework/Graphics/Camera.hpp"
#include "Framework/Graphics/RenderSubsystem.hpp"

namespace
{
	// HLSL의 row_major float4x4 두 개와 같은 저장 형식입니다.
	struct CameraConstants final
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};
}

namespace TUK::Game
{
	void TestWorld::OnEnter()
	{
		Entity& cameraEntity = CreateEntity();
		cameraEntity.SetName("Camera");
		cameraEntity.SetTag("Camera");

		Transform* cameraTransform = cameraEntity.AddComponent<Transform>();
		cameraTransform->SetLocalPosition(Vector3D<float>(0.0f, 0.0f, -2.0f));

		Camera* camera = cameraEntity.AddComponent<Camera>();
		assert(camera);
		
		renderSubsystem = Engine::GetInstance()->GetSubsystem<RenderSubsystem>();
		if (!renderSubsystem)
		{
			std::cerr << "TestWorld: RenderSubsystem이 없습니다." << std::endl;
			return;
		}

		const GPUDevice* device = renderSubsystem->GetGPUDevice();
		if (!device)
		{
			std::cerr << "TestWorld: GPUDevice가 없습니다." << std::endl;
			return;
		}

		camera->SetViewport(D3D12_VIEWPORT
		{
			0.0f, 0.0f, 1280.0f, 960.0f, 0.0f, 1.0f
		});
		camera->SetScissorRect(D3D12_RECT
		{
			0, 0, 1280, 960
		});
		if (auto result = CreateTriangle(*device, DXGI_FORMAT_R8G8B8A8_UNORM); !result)
		{
			std::cerr << "TestWorld 삼각형 생성 실패: " << result.error() << std::endl;
			return;
		}
	}

	void TestWorld::OnRender(GPUCommandBuffer& renderer)
	{
		if (!material || !pipelineState || !model || !cameraBuffer)
		{
			return;
		}

		const Camera* camera = GetMainCamera();
		if (!camera)
		{
			return;
		}

		CameraConstants constants{};
		DirectX::XMStoreFloat4x4(&constants.view, Matrix4x4<float>::Load(camera->GetViewMatrix()));
		DirectX::XMStoreFloat4x4(&constants.projection, Matrix4x4<float>::Load(camera->GetProjectionMatrix()));
		if (auto result = cameraBuffer->Write(std::as_bytes(std::span(&constants, 1))); !result)
		{
			std::cerr << "카메라 상수 갱신 실패: " << result.error() << std::endl;
			return;
		}

		renderer.SetGPUGraphicsPipelineState(*pipelineState);
		renderer.SetGPUCameraConstantBuffer(*cameraBuffer);
		renderer.SetViewport(camera->GetViewport());
		renderer.SetScissorRect(camera->GetScissorRect());
		const GPUMesh& mesh = model->GetMesh();
		renderer.SetGPUMesh(mesh);
		renderer.DrawIndexed(mesh.GetIndexCount());
	}

	void TestWorld::OnExit()
	{
		if (renderSubsystem)
		{
			if (material || model || pipelineState || cameraBuffer)
			{
				if (auto result = renderSubsystem->WaitForGPU(); !result)
				{
					std::cerr << "TestWorld GPU 종료 대기 실패: " << result.error() << std::endl;
				}
			}
		}

		material = nullptr;
		model = nullptr;
		cameraBuffer.reset();
		pipelineState.reset();
		renderSubsystem = nullptr;
	}

	std::expected<void, std::string> TestWorld::CreateTriangle(const GPUDevice& device, DXGI_FORMAT targetFormat)
	{
		auto* assetSubsystem = App::GetInstance()->GetSubsystem<AssetSubsystem>();
		if (!assetSubsystem)
		{
			return std::unexpected("AssetSubsystem이 없습니다.");
		}

		auto* vertexShader = assetSubsystem->LoadAsset<ShaderAsset>("Resources\\Shaders\\Triangle.vert.hlsl");
		auto* pixelShader = assetSubsystem->LoadAsset<ShaderAsset>("Resources\\Shaders\\Triangle.frag.hlsl");
		if (!vertexShader || !pixelShader)
		{
			return std::unexpected("삼각형 셰이더 에셋 로딩에 실패했습니다.");
		}

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;
		D3D12_RESOURCE_DESC bufferDescription{};
		bufferDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDescription.Width = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
		bufferDescription.Height = 1;
		bufferDescription.DepthOrArraySize = 1;
		bufferDescription.MipLevels = 1;
		bufferDescription.SampleDesc.Count = 1;
		bufferDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		auto newCameraBuffer = device.CreateGPUBuffer(heapProperties, bufferDescription, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!newCameraBuffer)
		{
			return std::unexpected(newCameraBuffer.error());
		}

		ModelAsset* newModel = assetSubsystem->LoadAsset<ModelAsset>("Resources\\Models\\Airplane.obj");
		if (!newModel)
		{
			return std::unexpected("삼각형 모델 에셋 로딩에 실패했습니다.");
		}

		MaterialAsset newMaterial(*vertexShader, *pixelShader);
		auto newPipelineState = CreateMaterialPipeline(device, newMaterial, targetFormat);
		if (!newPipelineState)
		{
			return std::unexpected(newPipelineState.error());
		}

		material = assetSubsystem->CreateAsset<MaterialAsset>(std::move(newMaterial));
		pipelineState = std::move(newPipelineState.value());
		model = newModel;
		cameraBuffer = std::move(newCameraBuffer.value());
		return {};
	}

	std::expected<GPUGraphicsPipelineState, std::string> TestWorld::CreateMaterialPipeline(
		const GPUDevice& device, const MaterialAsset& material, DXGI_FORMAT targetFormat) const
	{
		const std::array<D3D12_INPUT_ELEMENT_DESC, 2> inputElements
		{
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(ModelVertex, position)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "COLOR",
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(ModelVertex, color)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			},
		};
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineInfo{};
		pipelineInfo.VS = material.GetVertexShader().GetShader().GetD3DBytecode();
		pipelineInfo.PS = material.GetPixelShader().GetShader().GetD3DBytecode();
		pipelineInfo.InputLayout = { inputElements.data(), static_cast<UINT>(inputElements.size()) };
		pipelineInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineInfo.NumRenderTargets = 1;
		pipelineInfo.RTVFormats[0] = targetFormat;
		pipelineInfo.SampleMask = UINT_MAX;
		pipelineInfo.SampleDesc = { 1, 0 };
		pipelineInfo.RasterizerState.FillMode = material.GetFillMode();
		pipelineInfo.RasterizerState.CullMode = material.GetCullMode();
		pipelineInfo.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		pipelineInfo.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		pipelineInfo.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		pipelineInfo.RasterizerState.DepthClipEnable = TRUE;
		for (D3D12_RENDER_TARGET_BLEND_DESC& target : pipelineInfo.BlendState.RenderTarget)
		{
			target.SrcBlend = D3D12_BLEND_ONE;
			target.DestBlend = D3D12_BLEND_ZERO;
			target.BlendOp = D3D12_BLEND_OP_ADD;
			target.SrcBlendAlpha = D3D12_BLEND_ONE;
			target.DestBlendAlpha = D3D12_BLEND_ZERO;
			target.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			target.LogicOp = D3D12_LOGIC_OP_NOOP;
			target.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		}
		pipelineInfo.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipelineInfo.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineInfo.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		pipelineInfo.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		pipelineInfo.DepthStencilState.FrontFace = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
		pipelineInfo.DepthStencilState.BackFace = pipelineInfo.DepthStencilState.FrontFace;
		return device.CreateGPUGraphicsPipelineState(pipelineInfo);
	}

}
