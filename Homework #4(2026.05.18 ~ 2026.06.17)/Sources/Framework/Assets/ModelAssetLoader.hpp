#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <iosfwd>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "AssetLoader.hpp"
#include "ModelAsset.hpp"

#include "Framework/Math/ColorRGB.hpp"
#include "Framework/Math/Vector3D.hpp"

namespace TUK::Framework
{
	class GPUDevice;

	class ModelAssetLoader final : public AssetLoader
	{
	public:
		using AssetType = ModelAsset;

		explicit ModelAssetLoader(const GPUDevice& device) noexcept;
		~ModelAssetLoader() noexcept override = default;

		[[nodiscard]] std::expected<std::unique_ptr<Asset>, std::string> Load(
			const std::filesystem::path& path) const override;

	private:
		[[nodiscard]] bool ResolveIndex(std::string_view text, std::size_t count, std::size_t& result) const;

		[[nodiscard]] std::expected<void, std::string> ParseOBJVector(
			std::istringstream& stream, std::string_view type,
			std::vector<Vector3D<float>>& positions, std::vector<ColorRGB<float>>& colors,
			std::vector<Vector3D<float>>& normals, std::size_t lineNumber) const;
		[[nodiscard]] std::expected<void, std::string> ParseOBJFace(
			std::istringstream& stream,
			std::span<const Vector3D<float>> positions, std::span<const ColorRGB<float>> colors,
			std::span<const Vector3D<float>> normals,
			std::vector<ModelVertex>& vertices, std::vector<std::uint32_t>& indices, std::size_t lineNumber) const;

		[[nodiscard]] std::expected<void, std::string> ReadOBJ(
			const std::filesystem::path& path, std::vector<ModelVertex>& vertices, std::vector<std::uint32_t>& indices) const;

		[[nodiscard]] std::expected<GPUBuffer, std::string> CreateUploadBuffer(std::span<const std::byte> bytes) const;
		[[nodiscard]] std::expected<GPUMesh, std::string> CreateMesh(
			std::span<const ModelVertex> vertices, std::span<const std::uint32_t> indices) const;

		const GPUDevice& device;
	};
}
