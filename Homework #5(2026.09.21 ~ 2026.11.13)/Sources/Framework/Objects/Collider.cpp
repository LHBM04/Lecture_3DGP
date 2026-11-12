#include "Precompiled.hpp"
#include "Collider.hpp"

#ifdef _DEBUG
#include "../Core/App.hpp"

#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUConstants.hpp"

#include "../Resources/AssetSubsystem.hpp"
#include "../Resources/MaterialAsset.hpp"
#endif

#include "BoxCollider.hpp"
#include "GameObject.hpp"
#include "SphereCollider.hpp"
#include "Transform.hpp"

namespace TUK::Framework
{
#ifdef _DEBUG
	void Collider::OnAwake()
	{
		LoadDebugMaterial("Resources/Materials/DebugCollider.tukmat");
	}

	void Collider::LoadDebugMaterial(const std::filesystem::path& path)
	{
		if (AssetSubsystem* assets = App::GetInstance().GetSubsystem<AssetSubsystem>())
		{
			debugMaterial = assets->LoadAsset<MaterialAsset>(path);
		}
	}

	bool Collider::PrepareDebugRender(GPUCommandBuffer& commandBuffer, const Matrix4x4<f32>& world) const
	{
		if (!IsQueryEnabled() || !debugMaterial ||
			debugMaterial->GetTargetFormat() != commandBuffer.GetActiveTargetFormat())
		{
			return false;
		}

		const ObjectConstants constants{ .world = world };
		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
		{
			LOG_ERROR("콜라이더 기즈모 변환을 업로드하지 못했습니다: {}", result.error());
			return false;
		}

		commandBuffer.SetGPUGraphicsPipelineState(debugMaterial->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(debugMaterial->GetPrimitive());
		return true;
	}

	void Collider::DrawDebugLines(GPUCommandBuffer& commandBuffer, std::span<const Vector3D<f32>> vertices) const
	{
		struct alignas(16) DebugLineConstants final
		{
			ColorRGBA<f32> color{};
			std::array<DirectX::XMFLOAT4, 192> positions{};
		};

		ASSERT_FMT(vertices.size() <= 192, "콜라이더 기즈모 정점 수가 너무 많습니다.");
		ASSERT_FMT(vertices.size() % 2 == 0, "콜라이더 기즈모 선분은 짝수 개의 정점이 필요합니다.");

		if (!PrepareDebugRender(commandBuffer, Matrix4x4<f32>::GetIdentity()))
		{
			return;
		}

		DebugLineConstants constants{ .color = debugColor };
		for (usize index = 0; index < vertices.size(); ++index)
		{
			const auto& point = vertices[index];
			constants.positions[index] = DirectX::XMFLOAT4{ point.GetX(), point.GetY(), point.GetZ(), 1.0f };
		}

		if (auto result = commandBuffer.SetGraphicsConstants(2, constants); !result)
		{
			LOG_ERROR("콜라이더 기즈모 정점을 업로드하지 못했습니다: {}", result.error());
			return;
		}

		commandBuffer.Draw(static_cast<u32>(vertices.size()));
	}
#endif

	bool Collider::IsQueryEnabled() const noexcept
	{
		const GameObject* owner = GetOwner();
		return IsEnabled() && !IsDestroyed() &&
			owner && owner->IsEnabled() && !owner->IsDestroyed() && IsValid();
	}

	bool Collider::Intersects(const Collider* other) const noexcept
	{
		if (!other || other == this || !IsQueryEnabled() || !other->IsQueryEnabled())
		{
			return false;
		}

		if (!GetWorldBounds().Intersects(other->GetWorldBounds()))
		{
			return false;
		}

		if (const auto* sphere = dynamic_cast<const SphereCollider*>(other))
		{
			return Intersects(sphere->GetWorldSphere());
		}
		if (const auto* box = dynamic_cast<const BoxCollider*>(other))
		{
			return Intersects(box->GetWorldBox());
		}

		if (const auto* sphere = dynamic_cast<const SphereCollider*>(this))
		{
			return other->Intersects(sphere->GetWorldSphere());
		}
		if (const auto* box = dynamic_cast<const BoxCollider*>(this))
		{
			return other->Intersects(box->GetWorldBox());
		}

		return false;
	}

	Matrix4x4<f32> Collider::GetWorldMatrix() const noexcept
	{
		const GameObject* owner = GetOwner();
		const Transform* transform = owner ? owner->GetComponent<Transform>() : nullptr;
		return transform ? transform->GetWorldMatrix() : Matrix4x4<f32>::GetIdentity();
	}

	bool Collider::HasValidTransform() const noexcept
	{
		const auto world = GetWorldMatrix();
		Matrix4x4<f32> inverse{};
		return world[3] == 0.0f && world[7] == 0.0f && world[11] == 0.0f &&
			world[15] == 1.0f && world.TryGetInverse(inverse);
	}

	bool Collider::TryNormalizeRay(
		const Vector3D<f32>& origin,
		const Vector3D<f32>& direction,
		f32 maxDistance,
		Vector3D<f32>& normalizedDirection) noexcept
	{
		const f32 lengthSquared = direction.GetSqrMagnitude();
		if (!origin.IsFinite() || !direction.IsFinite() ||
			!std::isfinite(lengthSquared) || lengthSquared <= 0.0f ||
			!std::isfinite(maxDistance) || maxDistance < 0.0f)
		{
			return false;
		}

		normalizedDirection = direction / std::sqrt(lengthSquared);
		return true;
	}
}
