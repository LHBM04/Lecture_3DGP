#include "Precompiled.hpp"
#include "ButtonUI.hpp"

#include "../Core/App.hpp"

#include "../Objects/GameObject.hpp"
#include "../Objects/Transform.hpp"
#include "../Objects/Scene.hpp"

#include "../Graphics/Camera.hpp"
#ifdef _DEBUG
#include "../Graphics/GPUCommandBuffer.hpp"
#include "../Graphics/GPUConstants.hpp"
#endif

#include "../Platform/Display.hpp"
#include "../Platform/InputSubsystem.hpp"

#ifdef _DEBUG
#include "../Resources/MaterialAsset.hpp"
#endif

namespace TUK::Framework
{
	namespace
	{
		const Camera* FindUICamera(const Scene* scene) noexcept
		{
			for (const Camera* camera : scene->GetCameras() | std::views::reverse)
			{
				const GameObject* owner = camera->GetOwner();
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

			const Matrix4x4<f32> world = transform->GetWorldMatrix();
			const f32 determinant = world[0] * world[5] - world[1] * world[4];
			constexpr f32 MIN_DETERMINANT = 1.0e-6f;
			if (!std::isfinite(determinant) || std::abs(determinant) <= MIN_DETERMINANT)
			{
				return false;
			}

			const f32 offsetX = x - viewport.Width * 0.5f - world[12];
			const f32 offsetY = y - viewport.Height * 0.5f - world[13];
			const f32 localX = (offsetX * world[5] - offsetY * world[4]) / determinant;
			const f32 localY = (world[0] * offsetY - world[1] * offsetX) / determinant;
			return std::abs(localX) <= 0.5f && std::abs(localY) <= 0.5f;
		}
	}

	void ButtonUI::OnUpdate()
	{
		const GameObject* owner = GetOwner();
		const Scene* scene = owner->GetScene();
		const auto isActive = [this, owner, scene]()
		{
			return IsEnabled() && !IsDestroyed() && owner->IsEnabled() && !owner->IsDestroyed() && scene->IsEntered();
		};

		const InputSubsystem* input = App::GetInstance().GetSubsystem<InputSubsystem>();
		const InputProvider* provider = input ? input->GetProvider() : nullptr;
		const Display* display = input ? input->GetActiveDisplay() : nullptr;
		const Transform* transform = owner->GetComponent<Transform>();
		const Camera* camera = FindUICamera(scene);
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

#ifdef _DEBUG
	void ButtonUI::OnPostRender(GPUCommandBuffer& commandBuffer)
	{
		if (!debugMaterial)
		{
			return;
		}

		const Transform* transform = GetOwner()->GetComponent<Transform>();
		if (!transform)
		{
			return;
		}

		const ObjectConstants objectConstants
		{
			.world = transform->GetWorldMatrix()
		};
		if (auto result = commandBuffer.SetGraphicsConstants(objectConstants); !result)
		{
			return;
		}

		const D3D12_VIEWPORT& viewport = commandBuffer.GetActiveViewport();
		const Vector2D<f32> viewportSize = (viewport.Width > 0.0f && viewport.Height > 0.0f)
			? Vector2D<f32>{ viewport.Width, viewport.Height }
			: Vector2D<f32>{ 1280.0f, 960.0f };

		const ColorRGBA<f32> debugColor = isPressed
			? ColorRGBA<f32>::GetRed()
			: (isHovered ? ColorRGBA<f32>::GetYellow() : ColorRGBA<f32>::GetGreen());

		const ImageUIConstants constants
		{
			.viewportSize = viewportSize,
			.color = debugColor
		};
		if (auto result = commandBuffer.SetGraphicsConstants(constants); !result)
		{
			return;
		}

		commandBuffer.SetGPUGraphicsPipelineState(debugMaterial->GetGraphicsPipelineState());
		commandBuffer.SetPrimitiveTopology(debugMaterial->GetPrimitive());
		constexpr u32 DEBUG_BORDER_VERTEX_COUNT = 5;
		commandBuffer.Draw(DEBUG_BORDER_VERTEX_COUNT);
	}
#endif
}
