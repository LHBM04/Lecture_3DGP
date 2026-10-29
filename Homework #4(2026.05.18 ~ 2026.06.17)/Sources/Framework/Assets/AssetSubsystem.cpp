#include "Precompiled.hpp"

#include <system_error>

#include "AssetSubsystem.hpp"

#include "../Core/Engine.hpp"

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUShader.hpp"
#include "../Graphics/RenderSubsystem.hpp"

namespace TUK::Framework
{
	namespace
	{
		class ShaderAssetLoader final
		{
		public:
			[[nodiscard]] static std::expected<ShaderAsset, std::string> Load(
				const std::filesystem::path& path, const GPUDevice& device)
			{
				std::error_code error;
				if (!std::filesystem::is_regular_file(path, error))
				{
					return std::unexpected("셰이더 파일이 없거나 일반 파일로 접근할 수 없습니다.");
				}

				if (path.extension() != ".hlsl")
				{
					return std::unexpected("셰이더 소스는 .hlsl 확장자여야 합니다.");
				}

				GPUShaderCreateInfo createInfo{};
				createInfo.path = path;
				createInfo.entryPoint = "main";
				createInfo.compileFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;

				const auto stageExtension = path.stem().extension();
				if (stageExtension == ".vert")
				{
					createInfo.stage = ShaderStage::Vertex;
				}
				else if (stageExtension == ".tesc")
				{
					createInfo.stage = ShaderStage::Hull;
				}
				else if (stageExtension == ".tese")
				{
					createInfo.stage = ShaderStage::Domain;
				}
				else if (stageExtension == ".geom")
				{
					createInfo.stage = ShaderStage::Geometry;
				}
				else if (stageExtension == ".frag")
				{
					createInfo.stage = ShaderStage::Pixel;
				}
				else if (stageExtension == ".comp")
				{
					createInfo.stage = ShaderStage::Compute;
				}
				else
				{
					return std::unexpected(".hlsl 앞에 .vert, .tesc, .tese, .geom, .frag, .comp 중 하나를 지정해야 합니다.");
				}

				auto shader = device.CreateGPUShader(createInfo);
				if (!shader)
				{
					return std::unexpected(shader.error());
				}

				return ShaderAsset(std::move(*shader), std::move(createInfo));
			}
		};
	}

	SystemStatus AssetSubsystem::OnStartup()
	{
		for (auto iter : std::filesystem::recursive_directory_iterator("Resources\\Shaders"))
		{
			if (!iter.is_regular_file())
			{
				continue;
			}
			const auto& path = iter.path();
			if (path.extension() == ".hlsl")
			{
				LoadAsset<ShaderAsset>(path);
			}
		}
		return SystemStatus::Success;
	}

	SystemStatus AssetSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnFixedUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus AssetSubsystem::OnLateUpdate()
	{
		return SystemStatus::Continue;
	}

	void AssetSubsystem::OnShutdown()
	{
		assets.clear();
	}

	std::expected<ShaderAsset, std::string> AssetSubsystem::LoadShaderAsset(const std::filesystem::path& path)
	{
		const auto* renderSubsystem = Engine::GetInstance()->GetSubsystem<RenderSubsystem>();
		const GPUDevice* device = renderSubsystem ? renderSubsystem->GetGPUDevice() : nullptr;
		if (!device)
		{
			return std::unexpected("GPUDevice가 생성되지 않았습니다.");
		}

		return ShaderAssetLoader::Load(path, *device);
	}
}
