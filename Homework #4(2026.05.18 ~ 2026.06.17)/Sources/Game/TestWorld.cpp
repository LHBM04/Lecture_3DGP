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

		const GPUDevice* device = renderSubsystem->GetGPUDevice();
		if (!device)
		{
			std::cerr << "TestWorld: GPUDevice가 없습니다." << std::endl;
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

	void TestWorld::OnRender(GPUCommandBuffer& renderer)
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
		renderer.SetGPUGraphicsPipelineState(*pipelineState);
		renderer.SetGPUGraphicsConstantBuffer(0, *cameraBuffer);
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

		D3D12_ROOT_PARAMETER cameraParameter{};
		cameraParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		cameraParameter.Descriptor.ShaderRegister = 0;
		cameraParameter.Descriptor.RegisterSpace = 0;
		cameraParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		const GPURootSignatureCreateInfo rootSignatureInfo
		{
			.parameters = std::span(&cameraParameter, 1),
		};
		auto newRootSignature = device.CreateGPURootSignature(rootSignatureInfo);
		if (!newRootSignature)
		{
			return std::unexpected(newRootSignature.error());
		}

		const GPUBufferDescription cameraBufferInfo
		{
			.sizeInBytes = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT,
			.heapType = D3D12_HEAP_TYPE_UPLOAD,
		};
		auto newCameraBuffer = device.CreateGPUBuffer(cameraBufferInfo);
		if (!newCameraBuffer)
		{
			return std::unexpected(newCameraBuffer.error());
		}

		ModelAsset* newModel = assetSubsystem->LoadAsset<ModelAsset>("Resources\\Models\\Airplane.obj");
		if (!newModel)
		{
			return std::unexpected("삼각형 모델 에셋 로딩에 실패했습니다.");
		}

		const std::array<D3D12_INPUT_ELEMENT_DESC, 1> inputElements
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
		};
		const std::array<DXGI_FORMAT, 1> targetFormats{ targetFormat };
		const GPUGraphicsPipelineStateCreateInfo pipelineInfo
		{
			.rootSignature = newRootSignature.value(),
			.vertexShader = vertexShader->GetShader(),
			.pixelShader = pixelShader->GetShader(),
			.inputElements = inputElements,
			.renderTargetFormats = targetFormats,
		};
		auto newPipelineState = device.CreateGPUGraphicsPipelineState(pipelineInfo);
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
