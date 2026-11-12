#pragma once

#include "../Math/ColorRGBA.hpp"
#include "../Math/Vector2D.hpp"

#include "../Objects/Component.hpp"

namespace TUK::Framework
{
	class MaterialAsset;
	class TextureAsset;

	class BillboardRenderer final : public Component
	{
	public:
		using Component::Component;

		void OnUpdate() override;
		void OnRender(GPUCommandBuffer& commandBuffer) override;

		void Play(
			f32 duration,
			u32 columns = 1,
			u32 rows = 1,
			bool isLooping = false,
			bool isAutoDestroyOnFinish = true) noexcept;
		void Stop() noexcept;

		[[nodiscard]] const TextureAsset* GetTexture() const noexcept;
		void SetTexture(const TextureAsset* value) noexcept;

		[[nodiscard]] const MaterialAsset* GetMaterial() const noexcept;
		void SetMaterial(const MaterialAsset* value) noexcept;

		[[nodiscard]] const ColorRGBA<f32>& GetColor() const noexcept;
		void SetColor(const ColorRGBA<f32>& value) noexcept;

		[[nodiscard]] const Vector2D<f32>& GetSize() const noexcept;
		void SetSize(const Vector2D<f32>& value) noexcept;

		[[nodiscard]] u32 GetFrameIndex() const noexcept;
		void SetFrameIndex(u32 value) noexcept;

		[[nodiscard]] u32 GetColumns() const noexcept;
		void SetColumns(u32 value) noexcept;

		[[nodiscard]] u32 GetRows() const noexcept;
		void SetRows(u32 value) noexcept;

		[[nodiscard]] f32 GetDuration() const noexcept;
		void SetDuration(f32 value) noexcept;

		[[nodiscard]] bool IsPlaying() const noexcept;
		[[nodiscard]] bool IsLooping() const noexcept;
		void SetLooping(bool value) noexcept;

		[[nodiscard]] bool IsAutoDestroyOnFinish() const noexcept;
		void SetAutoDestroyOnFinish(bool value) noexcept;

	private:
		ColorRGBA<f32> color = ColorRGBA<f32>::GetWhite();
		Vector2D<f32> size{ 1.0f, 1.0f };
		u32 frameIndex = 0;
		u32 columns = 1;
		u32 rows = 1;

		f32 duration = 0.0f;
		f32 elapsedTime = 0.0f;
		bool isPlaying = false;
		bool isLooping = false;
		bool isAutoDestroyOnFinish = false;

		const TextureAsset* texture = nullptr;
		const MaterialAsset* material = nullptr;
	};

	inline const TextureAsset* BillboardRenderer::GetTexture() const noexcept
	{
		return texture;
	}

	inline void BillboardRenderer::SetTexture(const TextureAsset* value) noexcept
	{
		texture = value;
	}

	inline const MaterialAsset* BillboardRenderer::GetMaterial() const noexcept
	{
		return material;
	}

	inline void BillboardRenderer::SetMaterial(const MaterialAsset* value) noexcept
	{
		material = value;
	}

	inline const ColorRGBA<f32>& BillboardRenderer::GetColor() const noexcept
	{
		return color;
	}

	inline void BillboardRenderer::SetColor(const ColorRGBA<f32>& value) noexcept
	{
		color = value;
	}

	inline const Vector2D<f32>& BillboardRenderer::GetSize() const noexcept
	{
		return size;
	}

	inline void BillboardRenderer::SetSize(const Vector2D<f32>& value) noexcept
	{
		size = value;
	}

	inline u32 BillboardRenderer::GetFrameIndex() const noexcept
	{
		return frameIndex;
	}

	inline void BillboardRenderer::SetFrameIndex(u32 value) noexcept
	{
		frameIndex = value;
	}

	inline u32 BillboardRenderer::GetColumns() const noexcept
	{
		return columns;
	}

	inline void BillboardRenderer::SetColumns(u32 value) noexcept
	{
		columns = value;
	}

	inline u32 BillboardRenderer::GetRows() const noexcept
	{
		return rows;
	}

	inline void BillboardRenderer::SetRows(u32 value) noexcept
	{
		rows = value;
	}

	inline f32 BillboardRenderer::GetDuration() const noexcept
	{
		return duration;
	}

	inline void BillboardRenderer::SetDuration(f32 value) noexcept
	{
		duration = value;
	}

	inline bool BillboardRenderer::IsPlaying() const noexcept
	{
		return isPlaying;
	}

	inline bool BillboardRenderer::IsLooping() const noexcept
	{
		return isLooping;
	}

	inline void BillboardRenderer::SetLooping(bool value) noexcept
	{
		isLooping = value;
	}

	inline bool BillboardRenderer::IsAutoDestroyOnFinish() const noexcept
	{
		return isAutoDestroyOnFinish;
	}

	inline void BillboardRenderer::SetAutoDestroyOnFinish(bool value) noexcept
	{
		isAutoDestroyOnFinish = value;
	}
}
