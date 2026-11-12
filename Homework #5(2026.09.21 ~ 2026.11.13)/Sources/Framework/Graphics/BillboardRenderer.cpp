#include "Precompiled.hpp"
#include "BillboardRenderer.hpp"

#include "../Core/Common.hpp"
#include "../Core/Engine.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"

#include "../Platform/TimeSubsystem.hpp"

#include "../Resources/MaterialAsset.hpp"
#include "../Resources/TextureAsset.hpp"

#include "GPUCommandBuffer.hpp"
#include "GPUConstants.hpp"

namespace TUK::Framework
{
	void BillboardRenderer::Play(
		f32 newDuration,
		u32 newColumns,
		u32 newRows,
		bool isLoop,
		bool isAutoDestroy) noexcept
	{
		duration = std::max(0.0f, newDuration);
		columns = std::max(1u, newColumns);
		rows = std::max(1u, newRows);
		isLooping = isLoop;
		isAutoDestroyOnFinish = isAutoDestroy;
		elapsedTime = 0.0f;
		frameIndex = 0;
		isPlaying = (duration > 0.0f);
	}

	void BillboardRenderer::Stop() noexcept
	{
		isPlaying = false;
	}

	void BillboardRenderer::OnUpdate()
	{
		if (!isPlaying || duration <= 0.0f)
		{
			return;
		}

		const TimeSubsystem* timeSubsystem = Engine::GetInstance().GetSubsystem<TimeSubsystem>();
		const f32 deltaTime = timeSubsystem ? timeSubsystem->GetDeltaTime() : (1.0f / 60.0f);

		elapsedTime += deltaTime;
		const u32 totalFrames = columns * rows;
		if (totalFrames == 0)
		{
			return;
		}

		if (elapsedTime >= duration)
		{
			if (isLooping)
			{
				elapsedTime = std::fmod(elapsedTime, duration);
				const f32 progress = elapsedTime / duration;
				frameIndex = std::min(static_cast<u32>(progress * static_cast<f32>(totalFrames)), totalFrames - 1);
			}
			else
			{
				isPlaying = false;
				frameIndex = totalFrames - 1;
				if (isAutoDestroyOnFinish)
				{
					if (GameObject* owner = GetOwner())
					{
						owner->Destroy();
					}
				}
			}
		}
		else
		{
			const f32 progress = std::clamp(elapsedTime / duration, 0.0f, 1.0f);
			frameIndex = std::min(static_cast<u32>(progress * static_cast<f32>(totalFrames)), totalFrames - 1);
		}
	}

	void BillboardRenderer::OnRender(GPUCommandBuffer& commandBuffer)
	{
		if (!material || !texture)
		{
			return;
		}

		const GameObject* owner = GetOwner();
		const Transform* transform = owner ? owner->GetComponent<Transform>() : nullptr;
		const Matrix4x4<f32>& worldMatrix = transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity();

		const ObjectConstants objectConstants
		{
			.world = worldMatrix
		};

		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			LOG_ERROR("빌보드 변환을 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		const BillboardConstants constants
		{
			.size = size,
			.frameIndex = frameIndex,
			.columns = columns,
			.rows = rows,
			.padding = Vector3D<f32>{},
			.color = color
		};

		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
		{
			LOG_ERROR("빌보드 상수를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		std::array<const GPUTexture2D*, GPUCommandBuffer::TEXTURE_SLOT_COUNT> textures{};
		textures[0] = &texture->GetTexture();
		if (auto result = commandBuffer.BindTextures(textures); !result)
		{
			LOG_ERROR("빌보드 텍스처를 바인딩하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(material->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(material->GetPrimitive());

		constexpr u32 QUAD_VERTEX_COUNT = 4;
		commandBuffer.Draw(QUAD_VERTEX_COUNT);
	}
}
