#include "Precompiled.hpp"
#include "TestWorld.hpp"

#include "Framework/Assets/AssetSubsystem.hpp"
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

		const D3DDevice* device = renderSubsystem->GetD3DDevice();
		if (!device)
		{
			std::cerr << "TestWorld: D3DDevice가 없습니다." << std::endl;
			return;
		}

		camera->SetViewport(Viewport
		{
			0.0f, 0.0f, 1280.0f, 960.0f, 0.0f, 1.0f
		});
		camera->SetScissorRect(Rect
		{
			0, 0, 1280, 960
		});
		if (auto result = CreateTriangle(*device, DXGI_FORMAT_R8G8B8A8_UNORM); !result)
		{
			std::cerr << "TestWorld 삼각형 생성 실패: " << result.error() << std::endl;
			return;
		}
	}

	void TestWorld::OnRender(D3DCommandBuffer& renderer)
	{
		if (!rootSignature || !pipelineState || !model || !cameraBuffer)
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

		renderer.SetGPUGraphicsRootSignature(*rootSignature);
		renderer.SetD3DGraphicsPipelineState(*pipelineState);
		renderer.SetGPUGraphicsConstantBuffer(0, *cameraBuffer);
		renderer.SetViewport(camera->GetViewport());
		renderer.SetScissorRect(camera->GetScissorRect());
		const D3DMesh& mesh = model->GetMesh();
		renderer.SetD3DMesh(mesh);
		renderer.DrawIndexed(mesh.GetIndexCount());
	}

	void TestWorld::OnExit()
	{
		if (renderSubsystem)
		{
			if (model || pipelineState || rootSignature || cameraBuffer)
			{
				if (auto result = renderSubsystem->WaitForGPU(); !result)
				{
					std::cerr << "TestWorld GPU 종료 대기 실패: " << result.error() << std::endl;
				}
			}
		}

		model = nullptr;
		cameraBuffer.reset();
		pipelineState.reset();
		rootSignature.reset();
		renderSubsystem = nullptr;
	}

	std::expected<void, std::string> TestWorld::CreateTriangle(const D3DDevice& device, DXGI_FORMAT targetFormat)
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

		D3D12_ROOT_PARAMETER cameraParameter{};
		cameraParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		cameraParameter.Descriptor.ShaderRegister = 0;
		cameraParameter.Descriptor.RegisterSpace = 0;
		cameraParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureInfo{};
		rootSignatureInfo.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
		rootSignatureInfo.Desc_1_0.NumParameters = 1;
		rootSignatureInfo.Desc_1_0.pParameters = &cameraParameter;
		rootSignatureInfo.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		auto newRootSignature = device.CreateD3DRootSignature(rootSignatureInfo);
		if (!newRootSignature)
		{
			return std::unexpected(newRootSignature.error());
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
		auto newCameraBuffer = device.CreateD3DBuffer(heapProperties, bufferDescription, D3D12_RESOURCE_STATE_GENERIC_READ);
		if (!newCameraBuffer)
		{
			return std::unexpected(newCameraBuffer.error());
		}

		ModelAsset* newModel = assetSubsystem->LoadAsset<ModelAsset>("Resources\\Models\\Airplane.obj");
		if (!newModel)
		{
			return std::unexpected("삼각형 모델 에셋 로딩에 실패했습니다.");
		}

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
		pipelineInfo.pRootSignature = &newRootSignature->GetD3DRootSignature();
		pipelineInfo.VS = vertexShader->GetShader().GetD3DBytecode();
		pipelineInfo.PS = pixelShader->GetShader().GetD3DBytecode();
		pipelineInfo.InputLayout = { inputElements.data(), static_cast<UINT>(inputElements.size()) };
		pipelineInfo.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineInfo.NumRenderTargets = 1;
		pipelineInfo.RTVFormats[0] = targetFormat;
		pipelineInfo.SampleMask = UINT_MAX;
		pipelineInfo.SampleDesc = { 1, 0 };
		pipelineInfo.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipelineInfo.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
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
		auto newPipelineState = device.CreateD3DGraphicsPipelineState(pipelineInfo);
		if (!newPipelineState)
		{
			return std::unexpected(newPipelineState.error());
		}

		rootSignature = std::move(newRootSignature.value());
		pipelineState = std::move(newPipelineState.value());
		model = newModel;
		cameraBuffer = std::move(newCameraBuffer.value());
		return {};
	}
}
