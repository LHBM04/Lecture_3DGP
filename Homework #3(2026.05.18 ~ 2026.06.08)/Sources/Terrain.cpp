#include "Precompiled.h"

#include "Terrain.h"

#include "Logger.h"
#include "Mesh.h"
#include "RenderSystem.h"

bool Terrain::Load()
{
	if (path.empty())
	{
		Logger::Critical(L"[Terrain] Load failed: path is empty.");
		return false;
	}

	std::ifstream file{ path, std::ios::binary | std::ios::ate };
	if (!file.is_open())
	{
		Logger::Critical(L"[Terrain] Load failed: cannot open file. path={}", path);
		return false;
	}

	const std::streamsize fileSize{ file.tellg() };
	if (fileSize <= 0)
	{
		Logger::Critical(L"[Terrain] Load failed: file is empty. path={}", path);
		return false;
	}

	file.seekg(0, std::ios::beg);

	const uint64_t byteCount{ static_cast<uint64_t>(fileSize) };
	const uint32_t sizeAs8Bit{ static_cast<uint32_t>(std::sqrt(static_cast<double>(byteCount))) };
	const bool isSquare8Bit{ static_cast<uint64_t>(sizeAs8Bit) * sizeAs8Bit == byteCount };

	uint32_t sizeAs16Bit{ 0 };
	bool isSquare16Bit{ false };
	if ((byteCount % sizeof(uint16_t)) == 0)
	{
		const uint64_t sampleCount16Bit{ byteCount / sizeof(uint16_t) };
		sizeAs16Bit = static_cast<uint32_t>(std::sqrt(static_cast<double>(sampleCount16Bit)));
		isSquare16Bit = static_cast<uint64_t>(sizeAs16Bit) * sizeAs16Bit == sampleCount16Bit;
	}

	if (!isSquare8Bit && !isSquare16Bit)
	{
		Logger::Critical(L"[Terrain] Load failed: unsupported raw resolution. path={}, bytes={}", path, byteCount);
		return false;
	}

	is16Bit = !isSquare8Bit && isSquare16Bit;
	width = is16Bit ? sizeAs16Bit : sizeAs8Bit;
	height = width;

	const uint64_t sampleCount{ static_cast<uint64_t>(width) * height };
	samples.clear();
	samples.resize(static_cast<std::size_t>(sampleCount));

	if (is16Bit)
	{
		if (!static_cast<bool>(file.read(reinterpret_cast<char*>(samples.data()), static_cast<std::streamsize>(sampleCount * sizeof(uint16_t)))))
		{
			Logger::Critical(L"[Terrain] Load failed: cannot read 16-bit samples. path={}", path);
			return false;
		}
	}
	else
	{
		std::vector<uint8_t> bytes(static_cast<std::size_t>(sampleCount), 0);
		if (!static_cast<bool>(file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(sampleCount))))
		{
			Logger::Critical(L"[Terrain] Load failed: cannot read 8-bit samples. path={}", path);
			return false;
		}

		for (std::size_t i{ 0 }; i < bytes.size(); ++i)
		{
			samples[i] = bytes[i];
		}
	}

	if (!BuildMesh())
	{
		Logger::Critical(L"[Terrain] Load failed: mesh generation failed. path={}", path);
		return false;
	}

	Logger::Trace(L"[Terrain] Loaded. path={}, size={}x{}, bitDepth={}", path, width, height, is16Bit ? 16 : 8);
	return true;
}

void Terrain::Unload()
{
	samples.clear();
	mesh.reset();
	width = 0;
	height = 0;
	is16Bit = false;
}

Mesh* Terrain::GetMesh() const noexcept
{
	return mesh.get();
}

uint32_t Terrain::GetWidth() const noexcept
{
	return width;
}

uint32_t Terrain::GetHeight() const noexcept
{
	return height;
}

float Terrain::GetCellSize() const noexcept
{
	return cellSize;
}

float Terrain::GetHeightScale() const noexcept
{
	return heightScale;
}

float Terrain::GetHeightAt(uint32_t x_, uint32_t z_) const noexcept
{
	return SampleHeight(x_, z_);
}

bool Terrain::BuildMesh()
{
	if (width < 2 || height < 2 || samples.empty())
	{
		return false;
	}

	const std::size_t vertexCount{ static_cast<std::size_t>(width) * height };
	std::vector<Vertex> vertices(vertexCount);
	std::vector<uint32_t> indices;
	indices.reserve(static_cast<std::size_t>(width - 1) * (height - 1) * 6);

	const float halfWidth{ (static_cast<float>(width) - 1.0f) * cellSize * 0.5f };
	const float halfHeight{ (static_cast<float>(height) - 1.0f) * cellSize * 0.5f };

	for (uint32_t z{ 0 }; z < height; ++z)
	{
		for (uint32_t x{ 0 }; x < width; ++x)
		{
			const std::size_t index{ static_cast<std::size_t>(z) * width + x };
			vertices[index].position = Vector3D(
				static_cast<float>(x) * cellSize - halfWidth,
				SampleHeight(x, z),
				static_cast<float>(z) * cellSize - halfHeight);
			vertices[index].normal = CalculateNormal(x, z);
		}
	}

	for (uint32_t z{ 0 }; z < height - 1; ++z)
	{
		for (uint32_t x{ 0 }; x < width - 1; ++x)
		{
			const uint32_t topLeft{ z * width + x };
			const uint32_t topRight{ topLeft + 1 };
			const uint32_t bottomLeft{ (z + 1) * width + x };
			const uint32_t bottomRight{ bottomLeft + 1 };

			indices.emplace_back(topLeft);
			indices.emplace_back(bottomLeft);
			indices.emplace_back(topRight);

			indices.emplace_back(topRight);
			indices.emplace_back(bottomLeft);
			indices.emplace_back(bottomRight);
		}
	}

	mesh = std::make_unique<Mesh>();
	mesh->SetPath(path);
	mesh->SetName(name);
	mesh->SetVertices(std::move(vertices));
	mesh->SetIndices(std::move(indices));

	ID3D12Device* const device{ RenderSystem::GetInstance().GetDevice() };
	if (device != nullptr && !mesh->CreateBuffers(device))
	{
		return false;
	}

	return true;
}

float Terrain::SampleHeight(uint32_t x_, uint32_t z_) const noexcept
{
	if (samples.empty())
	{
		return 0.0f;
	}

	x_ = std::min(x_, width - 1);
	z_ = std::min(z_, height - 1);

	const std::size_t index{ static_cast<std::size_t>(z_) * width + x_ };
	const float maxValue{ is16Bit ? 65535.0f : 255.0f };
	return (static_cast<float>(samples[index]) / maxValue) * heightScale;
}

Vector3D Terrain::CalculateNormal(uint32_t x_, uint32_t z_) const noexcept
{
	const uint32_t left{ (x_ > 0) ? x_ - 1 : x_ };
	const uint32_t right{ (x_ + 1 < width) ? x_ + 1 : x_ };
	const uint32_t down{ (z_ > 0) ? z_ - 1 : z_ };
	const uint32_t up{ (z_ + 1 < height) ? z_ + 1 : z_ };

	const float heightL{ SampleHeight(left, z_) };
	const float heightR{ SampleHeight(right, z_) };
	const float heightD{ SampleHeight(x_, down) };
	const float heightU{ SampleHeight(x_, up) };

	Vector3D normal{ heightL - heightR, 2.0f * cellSize, heightD - heightU };
	normal.Normalize();
	return normal;
}
