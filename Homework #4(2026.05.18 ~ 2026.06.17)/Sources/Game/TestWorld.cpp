#include "Precompiled.hpp"
#include "TestWorld.hpp"

#include "Framework/Core/Engine.hpp"

#include "Framework/Entities/Entity.hpp"
#include "Framework/Entities/Transform.hpp"

#include "Framework/Graphics/Camera.hpp"
#include "Framework/Graphics/RenderSubsystem.hpp"

namespace
{
	struct Vertex final
	{
		float position[3];
		float color[4];
	};

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
		if (!rootSignature || !pipelineState || !mesh || !cameraBuffer)
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
		renderer.SetGPUMesh(*mesh);
		renderer.DrawIndexed(mesh->GetIndexCount());
	}

	void TestWorld::OnExit()
	{
		if (renderSubsystem)
		{
			if (mesh || pipelineState || rootSignature || cameraBuffer)
			{
				if (auto result = renderSubsystem->WaitForGPU(); !result)
				{
					std::cerr << "TestWorld GPU 종료 대기 실패: " << result.error() << std::endl;
				}
			}
		}

		mesh.reset();
		cameraBuffer.reset();
		pipelineState.reset();
		rootSignature.reset();
		renderSubsystem = nullptr;
	}

	std::expected<void, std::string> TestWorld::CreateTriangle(const GPUDevice& device, DXGI_FORMAT targetFormat)
	{
		// 실행 파일 옆 Resources/Shaders를 사용하므로 디버거의 작업 디렉터리에 의존하지 않습니다.
		std::array<char, 32768> executablePath{};
		const DWORD pathLength = GetModuleFileNameA(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
		if (pathLength == 0 || pathLength >= executablePath.size())
		{
			return std::unexpected("실행 파일 경로를 가져오지 못했습니다.");
		}
		const std::filesystem::path shaderPath = std::filesystem::path(executablePath.data()).parent_path() / "Resources" / "Shaders" / "Triangle.hlsl";

		GPUShaderCreateInfo shaderInfo
		{
			.path = shaderPath,
			.entryPoint = "VSMain",
			.target = "vs_5_1",
		};
#if defined(_DEBUG)
		shaderInfo.compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		auto vertexShader = device.CreateGPUShader(shaderInfo);
		if (!vertexShader)
		{
			return std::unexpected(vertexShader.error());
		}

		shaderInfo.entryPoint = "PSMain";
		shaderInfo.target = "ps_5_1";
		auto pixelShader = device.CreateGPUShader(shaderInfo);
		if (!pixelShader)
		{
			return std::unexpected(pixelShader.error());
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

		const std::array<Vertex, 3> vertices
		{
			Vertex{ { 0.0f, 0.6f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
			Vertex{ { 0.6f, -0.6f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
			Vertex{ { -0.6f, -0.6f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		};

		// 정점 세 개를 한 번만 기록하는 예제이므로 UPLOAD 버퍼를 직접 사용합니다.
		const GPUBufferDescription bufferInfo
		{
			.sizeInBytes = sizeof(vertices),
			.heapType = D3D12_HEAP_TYPE_UPLOAD,
		};
		auto vertexBuffer = device.CreateGPUBuffer(bufferInfo);
		if (!vertexBuffer)
		{
			return std::unexpected(vertexBuffer.error());
		}
		if (auto result = vertexBuffer->Write(std::as_bytes(std::span(vertices))); !result)
		{
			return std::unexpected(result.error());
		}

		const std::array<std::uint32_t, 3> indices{ 0, 1, 2 };
		const GPUBufferDescription indexBufferInfo
		{
			.sizeInBytes = sizeof(indices),
			.heapType = D3D12_HEAP_TYPE_UPLOAD,
		};
		auto indexBuffer = device.CreateGPUBuffer(indexBufferInfo);
		if (!indexBuffer)
		{
			return std::unexpected(indexBuffer.error());
		}
		if (auto result = indexBuffer->Write(std::as_bytes(std::span(indices))); !result)
		{
			return std::unexpected(result.error());
		}

		const GPUMeshCreateInfo meshInfo{ .vertexStride = static_cast<std::uint32_t>(sizeof(Vertex)) };
		auto newMesh = device.CreateGPUMesh(std::move(vertexBuffer.value()), std::move(indexBuffer.value()), meshInfo);
		if (!newMesh)
		{
			return std::unexpected(newMesh.error());
		}

		const std::array<D3D12_INPUT_ELEMENT_DESC, 2> inputElements
		{
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(Vertex, position)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			D3D12_INPUT_ELEMENT_DESC
			{
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = static_cast<std::uint32_t>(offsetof(Vertex, color)),
				.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
		};
		const std::array<DXGI_FORMAT, 1> targetFormats{ targetFormat };
		const GPUGraphicsPipelineStateCreateInfo pipelineInfo
		{
			.rootSignature = newRootSignature.value(),
			.vertexShader = vertexShader.value(),
			.pixelShader = &pixelShader.value(),
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
		mesh = std::move(newMesh.value());
		cameraBuffer = std::move(newCameraBuffer.value());
		return {};
	}
}
