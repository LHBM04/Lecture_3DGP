#include "Precompiled.hpp"
#include "ButtonUI.hpp"

#include <cmath>
#include <ranges>
#include <utility>

#include "../Core/App.hpp"

#include "../Entities/Entity.hpp"
#include "../Entities/Transform.hpp"
#include "../Entities/World.hpp"

#include "../Graphics/Camera.hpp"

#include "../Platform/Display.hpp"
#include "../Platform/InputSubsystem.hpp"

namespace TUK::Framework
{
	namespace
	{
		const Camera* FindUICamera(const World* world) noexcept
		{
			// UI uses the viewport left active by the last camera in the world render pass.
			for (const Camera* camera : world->GetCameras() | std::views::reverse)
			{
				const Entity* owner = camera->GetOwner();
				if (camera->IsEnabled() && !camera->IsDestroyed() && owner->IsEnabled() && !owner->IsDestroyed())
				{
					return camera;
				}
			}
			return nullptr;
		}

		bool HitTestButton(const Transform* transform, const Camera* camera, const Vector2D<i32>& cursor,
			const Vector2D<i32>& clientSize) noexcept
		{
			const auto& viewport = camera->GetViewport();
			const auto& scissor = camera->GetScissorRect();
			const i32 clientX = cursor.GetX();
			const i32 clientY = cursor.GetY();
			if (clientX < 0 || clientY < 0 || clientX >= clientSize.GetX() || clientY >= clientSize.GetY() ||
				clientX < scissor.left || clientY < scissor.top || clientX >= scissor.right || clientY >= scissor.bottom)
			{
				return false;
			}

			const f32 x = static_cast<f32>(clientX) - viewport.TopLeftX;
			const f32 y = static_cast<f32>(clientY) - viewport.TopLeftY;
			if (x < 0.0f || y < 0.0f || x >= viewport.Width || y >= viewport.Height)
			{
				return false;
			}

			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, Matrix4x4<f32>::Load(transform->GetWorldMatrix()));
			const f32 determinant = world._11 * world._22 - world._12 * world._21;
			constexpr f32 MIN_DETERMINANT = 1.0e-6f;
			if (!std::isfinite(determinant) || std::abs(determinant) <= MIN_DETERMINANT)
			{
				return false;
			}

			// ImageUI transforms a centered unit quad and uses only the resulting XY coordinates.
			const f32 offsetX = x - viewport.Width * 0.5f - world._41;
			const f32 offsetY = y - viewport.Height * 0.5f - world._42;
			const f32 localX = (offsetX * world._22 - offsetY * world._21) / determinant;
			const f32 localY = (world._11 * offsetY - world._12 * offsetX) / determinant;
			return std::abs(localX) <= 0.5f && std::abs(localY) <= 0.5f;
		}
	}

	void ButtonUI::OnUpdate()
	{
		const Entity* owner = GetOwner();
		const World* world = owner->GetWorld();
		const auto isActive = [this, owner, world]()
		{
			return IsEnabled() && !IsDestroyed() && owner->IsEnabled() && !owner->IsDestroyed() && world->IsEntered();
		};

		const InputSubsystem* input = App::GetInstance().GetSubsystem<InputSubsystem>();
		const InputProvider* provider = input ? input->GetProvider() : nullptr;
		const Display* display = input ? input->GetActiveDisplay() : nullptr;
		const Transform* transform = owner->GetComponent<Transform>();
		const Camera* camera = FindUICamera(world);
		if (!provider || !provider->HasMousePosition() || !display || !transform || !camera)
		{
			isPressed = false;
			if (std::exchange(isHovered, false))
			{
				onExit.Broadcast();
			}
			return;
		}

		const bool hoveredNow = HitTestButton(transform, camera, provider->GetMousePosition(), display->GetClientSize());
		const bool hasButtonState = provider->HasMouseButtonState();
		const bool pressedNow = hasButtonState && provider->IsMouseButtonPressed(ButtonCode::LEFT);
		const bool releasedNow = hasButtonState && provider->IsMouseButtonReleased(ButtonCode::LEFT);
		const bool heldNow = hasButtonState && provider->IsMouseButtonHeld(ButtonCode::LEFT);
		if (std::exchange(isHovered, hoveredNow) != hoveredNow)
		{
			(hoveredNow ? onEnter : onExit).Broadcast();
			if (!isActive())
			{
				return;
			}
		}

		if (pressedNow && isHovered)
		{
			isPressed = true;
			onDown.Broadcast();
		}
		else if (isPressed && !heldNow)
		{
			isPressed = false;
			if (releasedNow)
			{
				onUp.Broadcast();
				if (isHovered && isActive())
				{
					onClick.Broadcast();
				}
			}
		}
	}

	void ButtonUI::OnDisable()
	{
		isPressed = false;
		if (std::exchange(isHovered, false) && !IsDestroyed())
		{
			onExit.Broadcast();
		}
	}
}
