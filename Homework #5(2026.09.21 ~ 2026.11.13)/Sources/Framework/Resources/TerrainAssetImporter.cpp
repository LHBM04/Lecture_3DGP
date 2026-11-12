#include "Precompiled.hpp"
#include "TerrainAssetImporter.hpp"

#include <charconv>

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

		[[nodiscard]] constexpr bool IsAsciiWhitespace(char ch) noexcept
		{
			return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
		}

		[[nodiscard]] std::string_view Trim(std::string_view text) noexcept
		{
			constexpr std::string_view UTF8_BOM = "\xEF\xBB\xBF";
			if (text.starts_with(UTF8_BOM))
			{
				text.remove_prefix(UTF8_BOM.size());
			}
			while (!text.empty() && IsAsciiWhitespace(text.front()))
			{
				text.remove_prefix(1);
			}
			while (!text.empty() && IsAsciiWhitespace(text.back()))
			{
				text.remove_suffix(1);
			}
			return text;
		}

		template <typename TValue>
		[[nodiscard]] bool TryParseNumber(std::string_view text, TValue& outValue) noexcept
		{
			TValue parsed{};
			const char* begin = text.data();
			const char* end = begin + text.size();
			const auto [ptr, ec] = std::from_chars(begin, end, parsed);
			if (ec != std::errc{} || ptr != end)
			{
				return false;
			}

			outValue = parsed;
			return true;
		}

		[[nodiscard]] TerrainImportSettings LoadTerrainSettings(const std::filesystem::path& rawPath)
		{
			TerrainImportSettings settings{};
			std::filesystem::path metaPath = rawPath;
			metaPath.replace_extension(".meta");

			std::ifstream metaFile(metaPath);
			if (!metaFile)
			{
				return settings;
			}

			std::string line;
			while (std::getline(metaFile, line))
			{
				const std::string_view trimmed = Trim(line);
				if (trimmed.empty() || trimmed.starts_with('#') || trimmed.starts_with(';'))
				{
					continue;
				}

				const usize separator = trimmed.find('=');
				if (separator == std::string_view::npos)
				{
					continue;
				}

				const std::string_view key = Trim(trimmed.substr(0, separator));
				const std::string_view value = Trim(trimmed.substr(separator + 1));

				if (key == "width")
				{
					TryParseNumber(value, settings.width);
				}
				else if (key == "height")
				{
					TryParseNumber(value, settings.height);
				}
				else if (key == "bitDepth")
				{
					TryParseNumber(value, settings.bitDepth);
				}
				else if (key == "cellSize")
				{
					f32 parsed = 0.0f;
					if (TryParseNumber(value, parsed) && parsed > 0.0f)
					{
						settings.cellSize = parsed;
					}
				}
				else if (key == "heightScale")
				{
					TryParseNumber(value, settings.heightScale);
				}
				else if (key == "uvTilingX")
				{
					f32 parsed = 0.0f;
					if (TryParseNumber(value, parsed))
					{
						settings.uvTiling.SetX(parsed);
					}
				}
				else if (key == "uvTilingY")
				{
					f32 parsed = 0.0f;
					if (TryParseNumber(value, parsed))
					{
						settings.uvTiling.SetY(parsed);
					}
				}
			}

			return settings;
		}

		[[nodiscard]] bool HasRawExtension(const std::filesystem::path& path) noexcept
		{
			const std::string extension = path.extension().string();
			return extension == ".raw" || extension == ".RAW";
		}

		[[nodiscard]] u32 FindSquareResolution(u64 sampleCount) noexcept
		{
			if (sampleCount == 0)
			{
				return 0;
			}

			const u32 root = static_cast<u32>(std::sqrt(static_cast<f64>(sampleCount)));
			const u32 minCandidate = (root > 0) ? (root - 1) : 0;
			const u32 maxCandidate = root + 1;

			for (u32 candidate = minCandidate; candidate <= maxCandidate; ++candidate)
			{
				if (static_cast<u64>(candidate) * candidate == sampleCount)
				{
					return candidate;
				}
			}

			return 0;
		}

		template <typename TSample>
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
		if (!HasRawExtension(path))
		{
			return std::unexpected("지형 로더에는 .raw 높이맵 파일이 필요합니다.");
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

		const TerrainImportSettings settings = LoadTerrainSettings(path);
		const u64 byteCount = static_cast<u64>(fileSize);

		u32 width = settings.width;
		u32 height = settings.height;
		bool is16Bit = false;

		if (width >= 2 && height >= 2)
		{
			const u64 sampleCount = static_cast<u64>(width) * height;
			if (settings.bitDepth == 16 || (settings.bitDepth == 0 && byteCount == sampleCount * sizeof(u16)))
			{
				if (byteCount != sampleCount * sizeof(u16))
				{
					return std::unexpected("지형 높이맵 파일 크기가 16비트 해상도와 일치하지 않습니다: " + path.generic_string());
				}
				is16Bit = true;
			}
			else if (byteCount != sampleCount)
			{
				return std::unexpected("지형 높이맵 파일 크기가 해상도와 일치하지 않습니다: " + path.generic_string());
			}
		}
		else
		{
			const u32 sizeAs8Bit = (settings.bitDepth == 16) ? 0 : FindSquareResolution(byteCount);
			const u32 sizeAs16Bit = (settings.bitDepth != 8 && (byteCount % sizeof(u16)) == 0)
				? FindSquareResolution(byteCount / sizeof(u16))
				: 0;

			is16Bit = (sizeAs8Bit == 0) && (sizeAs16Bit != 0);
			width = is16Bit ? sizeAs16Bit : sizeAs8Bit;
			height = width;
		}

		if (width < 2 || height < 2)
		{
			return std::unexpected("지원되지 않는 높이맵 해상도입니다: " + path.generic_string());
		}

		const usize sampleCount = static_cast<usize>(width) * height;
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
