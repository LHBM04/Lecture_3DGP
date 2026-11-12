#include "Precompiled.hpp"
#include "TerrainAssetImporter.hpp"

#include "../Core/BinaryReader.hpp"

#include "../Graphics/GPUDevice.hpp"
#include "../Graphics/GPUMesh.hpp"
#include "../Graphics/MeshVertex.hpp"

namespace TUK::Framework
{
	namespace
	{
		struct TerrainImportSettings final
		{
			u32 width = 0;
			u32 height = 0;
			u32 bitDepth = 0;
			f32 cellSize = 4.0f;
			f32 heightScale = 120.0f;
			Vector2D<f32> uvTiling{ 1.0f, 1.0f };
		};

		[[nodiscard]] bool HasTerrainExtension(const std::filesystem::path& path) noexcept
		{
			const std::string extension = path.extension().string();
			return extension == ".tukterrain" || extension == ".TUKTERRAIN";
		}

		template <class TSample>
		[[nodiscard]] std::expected<std::vector<f32>, std::string> ReadAndNormalizeSamples(
			std::ifstream& file,
			usize sampleCount,
			const std::filesystem::path& path)
		{
			std::vector<TSample> rawSamples(sampleCount, 0);
			if (!static_cast<bool>(file.read(
				reinterpret_cast<char*>(rawSamples.data()),
				static_cast<std::streamsize>(sampleCount * sizeof(TSample)))))
			{
				return std::unexpected("높이맵 데이터를 읽지 못했습니다: " + path.generic_string());
			}

			constexpr f32 MAX_SAMPLE_VALUE = static_cast<f32>(std::numeric_limits<TSample>::max());
			std::vector<f32> normalizedHeights(sampleCount, 0.0f);
			for (usize index = 0; index < sampleCount; ++index)
			{
				normalizedHeights[index] = static_cast<f32>(rawSamples[index]) / MAX_SAMPLE_VALUE;
			}

			return normalizedHeights;
		}

		void BuildTerrainMeshData(
			std::span<const f32> normalizedHeights,
			u32 width,
			u32 height,
			f32 cellSize,
			f32 heightScale,
			const Vector2D<f32>& uvTiling,
			std::vector<MeshVertex>& outVertices,
			std::vector<u32>& outIndices)
		{
			const usize vertexCount = static_cast<usize>(width) * height;
			const usize indexCount = static_cast<usize>(width - 1) * (height - 1) * 6;

			outVertices.resize(vertexCount);
			outIndices.clear();
			outIndices.reserve(indexCount);

			const f32 halfWidth = static_cast<f32>(width - 1) * cellSize * 0.5f;
			const f32 halfDepth = static_cast<f32>(height - 1) * cellSize * 0.5f;
			const f32 invMaxX = 1.0f / static_cast<f32>(width - 1);
			const f32 invMaxZ = 1.0f / static_cast<f32>(height - 1);

			for (u32 z = 0; z < height; ++z)
			{
				for (u32 x = 0; x < width; ++x)
				{
					const usize index = static_cast<usize>(z) * width + x;
					const f32 posX = static_cast<f32>(x) * cellSize - halfWidth;
					const f32 posY = normalizedHeights[index] * heightScale;
					const f32 posZ = static_cast<f32>(z) * cellSize - halfDepth;
					const Vector3D<f32> normal = TerrainAsset::CalculateGridNormal(
						normalizedHeights,
						width,
						height,
						x,
						z,
						cellSize,
						heightScale);

					outVertices[index] = MeshVertex
					{
						.position = Vector3D<f32>{ posX, posY, posZ },
						.normal = normal,
						.color = ColorRGBA<f32>::GetWhite(),
						.textureCoordinate = Vector2D<f32>
						{
							static_cast<f32>(x) * invMaxX * uvTiling.GetX(),
							static_cast<f32>(z) * invMaxZ * uvTiling.GetY()
						}
					};
				}
			}

			for (u32 z = 0; z < height - 1; ++z)
			{
				for (u32 x = 0; x < width - 1; ++x)
				{
					const u32 topLeft = z * width + x;
					const u32 topRight = topLeft + 1;
					const u32 bottomLeft = (z + 1) * width + x;
					const u32 bottomRight = bottomLeft + 1;

					outIndices.emplace_back(topLeft);
					outIndices.emplace_back(bottomLeft);
					outIndices.emplace_back(topRight);

					outIndices.emplace_back(topRight);
					outIndices.emplace_back(bottomLeft);
					outIndices.emplace_back(bottomRight);
				}
			}
		}
	}

	std::expected<std::unique_ptr<TerrainAsset>, std::string> TerrainAssetImporter::Import(
		const std::filesystem::path& path) const
	{
		if (!HasTerrainExtension(path))
		{
			return std::unexpected("지형 로더에는 .tukterrain 높이맵 파일이 필요합니다.");
		}

		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			return std::unexpected("지형 높이맵 파일을 열 수 없습니다: " + path.generic_string());
		}

		const std::streamsize fileSize = file.tellg();
		if (fileSize <= 0)
		{
			return std::unexpected("지형 높이맵 파일이 비어 있습니다: " + path.generic_string());
		}
		file.seekg(0, std::ios::beg);

		constexpr u64 HEADER_SIZE = 3 * sizeof(u32) + 4 * sizeof(f32);
		BinaryReader reader(file);
		TerrainImportSettings settings{};
		f32 uvTilingX = 0.0f;
		f32 uvTilingY = 0.0f;
		if (!reader.Read(settings.width, settings.height, settings.bitDepth,
			settings.cellSize, settings.heightScale, uvTilingX, uvTilingY))
		{
			return std::unexpected("지형 헤더를 읽지 못했습니다: " + path.generic_string());
		}

		if (settings.width < 2 || settings.height < 2
			|| (settings.bitDepth != 8 && settings.bitDepth != 16)
			|| settings.cellSize <= 0.0f || settings.heightScale < 0.0f
			|| uvTilingX <= 0.0f || uvTilingY <= 0.0f)
		{
			return std::unexpected("지형 헤더의 설정값이 유효하지 않습니다: " + path.generic_string());
		}
		settings.uvTiling = Vector2D<f32>{ uvTilingX, uvTilingY };

		const u32 width = settings.width;
		const u32 height = settings.height;
		const bool is16Bit = settings.bitDepth == 16;
		const u64 sampleCount64 = static_cast<u64>(width) * height;
		const u64 sampleSize = is16Bit ? sizeof(u16) : sizeof(u8);
		if (sampleCount64 > BinaryReader::MAX_ELEMENTS
			|| static_cast<u64>(fileSize) != HEADER_SIZE + sampleCount64 * sampleSize)
		{
			return std::unexpected("지형 높이 데이터 크기가 해상도 및 샘플 형식과 일치하지 않습니다: " + path.generic_string());
		}

		const usize sampleCount = static_cast<usize>(sampleCount64);
		auto normalizedHeights = is16Bit
			? ReadAndNormalizeSamples<u16>(file, sampleCount, path)
			: ReadAndNormalizeSamples<u8>(file, sampleCount, path);
		if (!normalizedHeights)
		{
			return std::unexpected(normalizedHeights.error());
		}

		std::vector<MeshVertex> vertices;
		std::vector<u32> indices;
		BuildTerrainMeshData(
			*normalizedHeights,
			width,
			height,
			settings.cellSize,
			settings.heightScale,
			settings.uvTiling,
			vertices,
			indices);

		auto gpuMesh = device.CreateMesh(
			std::as_bytes(std::span<const MeshVertex>(vertices)),
			sizeof(MeshVertex),
			std::as_bytes(std::span<const u32>(indices)),
			DXGI_FORMAT_R32_UINT,
			D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		if (!gpuMesh)
		{
			return std::unexpected(gpuMesh.error());
		}

		return std::make_unique<TerrainAsset>(
			path,
			width,
			height,
			is16Bit,
			settings.cellSize,
			settings.heightScale,
			settings.uvTiling,
			std::move(*normalizedHeights),
			std::move(*gpuMesh));
	}
}
