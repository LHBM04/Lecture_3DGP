#include "Precompiled.hpp"
#include "Renderer.hpp"

#include "Camera.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"
#include "Object.hpp"
#include "Transform.hpp"

int Renderer::GetWidth()
{
	return options.width;
}

void Renderer::SetWidth(int width_)
{
	Resize(width_, options.height);
}

int Renderer::GetHeight()
{
	return options.height;
}

void Renderer::SetHeight(int height_)
{
	Resize(options.width, height_);
}

void Renderer::Resize(int width_, int height_)
{
	options.width = std::max(width_, 1);
	options.height = std::max(height_, 1);

	colorBuffer.resize(static_cast<std::size_t>(options.width) * static_cast<std::size_t>(options.height));
	zBuffer.assign(static_cast<std::size_t>(options.width) * static_cast<std::size_t>(options.height), std::numeric_limits<float>::infinity());

	RecreateBackBuffer();
}

void Renderer::SetPolygonMode(PolygonMode polygonMode_)
{
	polygonMode = polygonMode_;
}

Renderer::PolygonMode Renderer::GetPolygonMode()
{
	return polygonMode;
}

void Renderer::SetCamera(Camera* camera_)
{
	camera = camera_;
}

Camera* Renderer::GetCamera()
{
	return camera;
}

void Renderer::SetObject(Object* object_)
{
	object = object_;
}

Object* Renderer::GetObject()
{
	return object;
}

bool Renderer::Initialize(const Options& options_)
{
	options = options_;
	options.width = std::max(options.width, 1);
	options.height = std::max(options.height, 1);

	windowDC = GetDC(options.hWnd);
	if (!windowDC)
	{
		Logger::Error("Device Context를 가져오는 데 실패했습니다!");
		return false;
	}

	backBufferDC = CreateCompatibleDC(windowDC);
	if (!backBufferDC)
	{
		Logger::Error("백버퍼 Device Context를 생성하는 데 실패했습니다!");
		ReleaseDC(options.hWnd, windowDC);
		windowDC = nullptr;
		return false;
	}

	Resize(options.width, options.height);
	return backBufferBitmap != nullptr;
}

void Renderer::Terminate()
{
	colorBuffer.clear();
	colorBuffer.shrink_to_fit();

	zBuffer.clear();
	zBuffer.shrink_to_fit();

	meshCommands.clear();
	meshCommands.shrink_to_fit();

	textCommands.clear();
	textCommands.shrink_to_fit();

	if (backBufferDC)
	{
		if (previousBackBufferBitmap)
		{
			SelectObject(backBufferDC, previousBackBufferBitmap);
			previousBackBufferBitmap = nullptr;
		}

		if (backBufferBitmap)
		{
			DeleteObject(backBufferBitmap);
			backBufferBitmap = nullptr;
		}

		DeleteDC(backBufferDC);
		backBufferDC = nullptr;
	}

	if (windowDC)
	{
		ReleaseDC(options.hWnd, windowDC);
		windowDC = nullptr;
	}
}

void Renderer::Reset()
{
	const unsigned int clearColor = ToPackedColor(options.clearColor);
	std::fill(colorBuffer.begin(), colorBuffer.end(), clearColor);
	std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::infinity());
	meshCommands.clear();
	textCommands.clear();
}

void Renderer::QueueMesh(Mesh* mesh_, const ColorRGBA& color_)
{
	if (!mesh_)
	{
		return;
	}

	MeshCommand meshCommand{};
	meshCommand.mesh = mesh_;
	meshCommand.camera = camera;
	meshCommand.object = object;
	meshCommand.color = color_;
	meshCommands.push_back(meshCommand);
}

void Renderer::QueueText(const TextCommand& textCommand_)
{
	textCommands.push_back(textCommand_);
}

unsigned int Renderer::ToPackedColor(const ColorRGBA& color_)
{
	const auto r = static_cast<unsigned char>(std::clamp(color_.x, 0.0f, 1.0f) * 255.0f);
	const auto g = static_cast<unsigned char>(std::clamp(color_.y, 0.0f, 1.0f) * 255.0f);
	const auto b = static_cast<unsigned char>(std::clamp(color_.z, 0.0f, 1.0f) * 255.0f);
	const auto a = static_cast<unsigned char>(std::clamp(color_.w, 0.0f, 1.0f) * 255.0f);

	return (static_cast<unsigned int>(a) << 24)
		| (static_cast<unsigned int>(r) << 16)
		| (static_cast<unsigned int>(g) << 8)
		| (static_cast<unsigned int>(b));
}

float Renderer::EdgeFunction(float ax_, float ay_, float bx_, float by_, float px_, float py_)
{
	return (px_ - ax_) * (by_ - ay_) - (py_ - ay_) * (bx_ - ax_);
}

void Renderer::DrawTrianglesByArrays(const std::vector<RasterVertex>& vertices_)
{
	for (std::size_t index = 0; index + 2 < vertices_.size(); index += 3)
	{
		const RasterVertex& v0 = vertices_[index + 0];
		const RasterVertex& v1 = vertices_[index + 1];
		const RasterVertex& v2 = vertices_[index + 2];
		if (v0.z < CLIP_Z_NEAR || v0.z > CLIP_Z_FAR ||
			v1.z < CLIP_Z_NEAR || v1.z > CLIP_Z_FAR ||
			v2.z < CLIP_Z_NEAR || v2.z > CLIP_Z_FAR)
		{
			continue;
		}

		const ColorRGBA averageColor(
			(v0.color.x + v1.color.x + v2.color.x) / 3.0f,
			(v0.color.y + v1.color.y + v2.color.y) / 3.0f,
			(v0.color.z + v1.color.z + v2.color.z) / 3.0f,
			(v0.color.w + v1.color.w + v2.color.w) / 3.0f);
		const unsigned int packedColor = ToPackedColor(averageColor);

		if (polygonMode == PolygonMode::Wire)
		{
			DrawLineScreen(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, packedColor);
			DrawLineScreen(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, packedColor);
			DrawLineScreen(v2.x, v2.y, v2.z, v0.x, v0.y, v0.z, packedColor);
		}
		else
		{
			DrawTriangleFilledScreen(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, packedColor);
		}
	}
}

void Renderer::DrawTrianglesByElements(const std::vector<RasterVertex>& vertices_, const std::vector<std::uint32_t>& indices_)
{
	for (std::size_t index = 0; index + 2 < indices_.size(); index += 3)
	{
		const std::uint32_t i0 = indices_[index + 0];
		const std::uint32_t i1 = indices_[index + 1];
		const std::uint32_t i2 = indices_[index + 2];
		if (i0 >= vertices_.size() || i1 >= vertices_.size() || i2 >= vertices_.size())
		{
			continue;
		}

		const RasterVertex& v0 = vertices_[i0];
		const RasterVertex& v1 = vertices_[i1];
		const RasterVertex& v2 = vertices_[i2];
		if (v0.z < CLIP_Z_NEAR || v0.z > CLIP_Z_FAR ||
			v1.z < CLIP_Z_NEAR || v1.z > CLIP_Z_FAR ||
			v2.z < CLIP_Z_NEAR || v2.z > CLIP_Z_FAR)
		{
			continue;
		}

		const ColorRGBA averageColor(
			(v0.color.x + v1.color.x + v2.color.x) / 3.0f,
			(v0.color.y + v1.color.y + v2.color.y) / 3.0f,
			(v0.color.z + v1.color.z + v2.color.z) / 3.0f,
			(v0.color.w + v1.color.w + v2.color.w) / 3.0f);
		const unsigned int packedColor = ToPackedColor(averageColor);

		if (polygonMode == PolygonMode::Wire)
		{
			DrawLineScreen(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, packedColor);
			DrawLineScreen(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, packedColor);
			DrawLineScreen(v2.x, v2.y, v2.z, v0.x, v0.y, v0.z, packedColor);
		}
		else
		{
			DrawTriangleFilledScreen(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, packedColor);
		}
	}
}

void Renderer::Present()
{
	if (!windowDC || !backBufferDC || !backBufferBitmap || colorBuffer.empty())
	{
		meshCommands.clear();
		textCommands.clear();
		return;
	}

	for (const MeshCommand& meshCommand : meshCommands)
	{
		RasterizeMeshCommand(meshCommand);
	}

	BITMAPINFO bitmapInfo{};
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth = options.width;
	bitmapInfo.bmiHeader.biHeight = -options.height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	if (SetDIBitsToDevice(backBufferDC, 0, 0, options.width, options.height, 0, 0, 0, options.height, colorBuffer.data(), &bitmapInfo, DIB_RGB_COLORS) == 0)
	{
		Logger::Error("화면에 비트맵을 출력하는 데 실패했습니다! 오류 코드: %d", GetLastError());
	}

	for (const TextCommand& textCommand : textCommands)
	{
		HGDIOBJ previousFont = textCommand.font ? SelectObject(backBufferDC, textCommand.font) : nullptr;
		const int previousBkMode = SetBkMode(backBufferDC, TRANSPARENT);

		if (textCommand.drawShadow)
		{
			RECT shadowRect = textCommand.rect;
			OffsetRect(&shadowRect, textCommand.shadowOffset.x, textCommand.shadowOffset.y);
			SetTextColor(backBufferDC, static_cast<COLORREF>(textCommand.shadowColor));
			DrawTextW(backBufferDC, textCommand.text.c_str(), -1, &shadowRect, textCommand.format);
		}

		RECT drawRect = textCommand.rect;
		SetTextColor(backBufferDC, static_cast<COLORREF>(textCommand.textColor));
		DrawTextW(backBufferDC, textCommand.text.c_str(), -1, &drawRect, textCommand.format);

		SetBkMode(backBufferDC, previousBkMode);
		if (previousFont)
		{
			SelectObject(backBufferDC, previousFont);
		}
	}

	if (!BitBlt(windowDC, 0, 0, options.width, options.height, backBufferDC, 0, 0, SRCCOPY))
	{
		Logger::Error("백버퍼를 화면에 출력하는 데 실패했습니다! 오류 코드: %d", GetLastError());
	}

	meshCommands.clear();
	textCommands.clear();
}

bool Renderer::RecreateBackBuffer()
{
	if (!backBufferDC || !windowDC)
	{
		return false;
	}

	if (previousBackBufferBitmap)
	{
		SelectObject(backBufferDC, previousBackBufferBitmap);
		previousBackBufferBitmap = nullptr;
	}

	if (backBufferBitmap)
	{
		DeleteObject(backBufferBitmap);
		backBufferBitmap = nullptr;
	}

	backBufferBitmap = CreateCompatibleBitmap(windowDC, options.width, options.height);
	if (!backBufferBitmap)
	{
		Logger::Error("백버퍼 비트맵을 생성하는 데 실패했습니다! 오류 코드: %d", GetLastError());
		return false;
	}

	previousBackBufferBitmap = SelectObject(backBufferDC, backBufferBitmap);
	return previousBackBufferBitmap != nullptr;
}

void Renderer::RasterizeMeshCommand(const MeshCommand& meshCommand_)
{
	if (!meshCommand_.mesh || !meshCommand_.camera || !meshCommand_.object)
	{
		return;
	}

	const Transform* transform = meshCommand_.object->GetTransform();
	if (!transform)
	{
		return;
	}

	const auto& vertices = meshCommand_.mesh->GetVertices();
	if (vertices.empty())
	{
		return;
	}

	const int screenWidth = std::max(options.width, 1);
	const int screenHeight = std::max(options.height, 1);
	const float aspectRatio = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);
	const Matrix4x4 worldMatrix = transform->GetWorldMatrix();
	const Matrix4x4 viewProjectionMatrix = meshCommand_.camera->GetViewProjectionMatrix(aspectRatio);

	std::vector<RasterVertex> rasterVertices;
	rasterVertices.reserve(vertices.size());

	for (const Mesh::Vertex& vertex : vertices)
	{
		const Vector3D worldPosition = worldMatrix.MultiplyPoint(vertex.position);
		const Vector3D clipPosition = viewProjectionMatrix.MultiplyPoint(worldPosition);

		RasterVertex rasterVertex{};
		rasterVertex.color = ColorRGBA(
			vertex.color.x * meshCommand_.color.x,
			vertex.color.y * meshCommand_.color.y,
			vertex.color.z * meshCommand_.color.z,
			meshCommand_.color.w);

		const bool isClipValid =
			std::isfinite(clipPosition.x) &&
			std::isfinite(clipPosition.y) &&
			std::isfinite(clipPosition.z) &&
			std::abs(clipPosition.x) <= 10000.0f &&
			std::abs(clipPosition.y) <= 10000.0f &&
			clipPosition.z >= CLIP_Z_NEAR &&
			clipPosition.z <= CLIP_Z_FAR;

		if (!isClipValid)
		{
			rasterVertex.x = 0;
			rasterVertex.y = 0;
			rasterVertex.z = CLIP_Z_FAR + 1.0f;
		}
		else
		{
			rasterVertex.x = static_cast<int>((clipPosition.x * 0.5f + 0.5f) * static_cast<float>(screenWidth - 1));
			rasterVertex.y = static_cast<int>((1.0f - (clipPosition.y * 0.5f + 0.5f)) * static_cast<float>(screenHeight - 1));
			rasterVertex.z = clipPosition.z;
		}

		rasterVertices.push_back(rasterVertex);
	}

	const auto& indices = meshCommand_.mesh->GetIndices();
	if (!indices.empty())
	{
		DrawTrianglesByElements(rasterVertices, indices);
	}
	else
	{
		DrawTrianglesByArrays(rasterVertices);
	}
}

void Renderer::DrawLineScreen(int x0_, int y0_, float z0_, int x1_, int y1_, float z1_, unsigned int color_)
{
	const int deltaX = x1_ - x0_;
	const int deltaY = y1_ - y0_;
	const int steps = std::max(std::abs(deltaX), std::abs(deltaY));

	if (steps <= 0)
	{
		DrawPixelScreen(x0_, y0_, z0_, color_);
		return;
	}

	for (int index = 0; index <= steps; ++index)
	{
		const float t = static_cast<float>(index) / static_cast<float>(steps);
		const int x = static_cast<int>(std::round(static_cast<float>(x0_) + static_cast<float>(deltaX) * t));
		const int y = static_cast<int>(std::round(static_cast<float>(y0_) + static_cast<float>(deltaY) * t));
		const float z = z0_ + (z1_ - z0_) * t;

		DrawPixelScreen(x, y, z, color_);
	}
}

void Renderer::DrawTriangleFilledScreen(
	int x0_, int y0_, float z0_,
	int x1_, int y1_, float z1_,
	int x2_, int y2_, float z2_,
	unsigned int color_)
{
	const int minX = std::max(0, std::min({ x0_, x1_, x2_ }));
	const int maxX = std::min(options.width - 1, std::max({ x0_, x1_, x2_ }));
	const int minY = std::max(0, std::min({ y0_, y1_, y2_ }));
	const int maxY = std::min(options.height - 1, std::max({ y0_, y1_, y2_ }));

	const float area = EdgeFunction(
		static_cast<float>(x0_), static_cast<float>(y0_),
		static_cast<float>(x1_), static_cast<float>(y1_),
		static_cast<float>(x2_), static_cast<float>(y2_));
	if (std::abs(area) <= Mathf::EPSILON)
	{
		return;
	}

	for (int y = minY; y <= maxY; ++y)
	{
		for (int x = minX; x <= maxX; ++x)
		{
			const float sampleX = static_cast<float>(x) + 0.5f;
			const float sampleY = static_cast<float>(y) + 0.5f;

			const float w0 = EdgeFunction(
				static_cast<float>(x1_), static_cast<float>(y1_),
				static_cast<float>(x2_), static_cast<float>(y2_),
				sampleX, sampleY);
			const float w1 = EdgeFunction(
				static_cast<float>(x2_), static_cast<float>(y2_),
				static_cast<float>(x0_), static_cast<float>(y0_),
				sampleX, sampleY);
			const float w2 = EdgeFunction(
				static_cast<float>(x0_), static_cast<float>(y0_),
				static_cast<float>(x1_), static_cast<float>(y1_),
				sampleX, sampleY);

			const bool isInsideCCW = (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f);
			const bool isInsideCW = (w0 <= 0.0f && w1 <= 0.0f && w2 <= 0.0f);
			if (!isInsideCCW && !isInsideCW)
			{
				continue;
			}

			const float invArea = 1.0f / area;
			const float b0 = w0 * invArea;
			const float b1 = w1 * invArea;
			const float b2 = w2 * invArea;
			const float depth = b0 * z0_ + b1 * z1_ + b2 * z2_;

			DrawPixelScreen(x, y, depth, color_);
		}
	}
}

void Renderer::DrawPixelScreen(int x_, int y_, float z_, unsigned int color_)
{
	if (x_ < 0 || x_ >= options.width || y_ < 0 || y_ >= options.height)
	{
		return;
	}

	if (z_ < CLIP_Z_NEAR || z_ > CLIP_Z_FAR)
	{
		return;
	}

	const std::size_t index = static_cast<std::size_t>(y_) * static_cast<std::size_t>(options.width) + static_cast<std::size_t>(x_);
	if (index >= zBuffer.size() || index >= colorBuffer.size())
	{
		return;
	}

	if (z_ >= zBuffer[index])
	{
		return;
	}

	zBuffer[index] = z_;
	colorBuffer[index] = color_;
}

Renderer::Options Renderer::options{};

Renderer::PolygonMode Renderer::polygonMode{ PolygonMode::Wire };

Camera* Renderer::camera{ nullptr };
Object* Renderer::object{ nullptr };

HDC Renderer::windowDC{ nullptr };
HDC Renderer::backBufferDC{ nullptr };
HBITMAP Renderer::backBufferBitmap{ nullptr };
HGDIOBJ Renderer::previousBackBufferBitmap{ nullptr };

std::vector<unsigned int> Renderer::colorBuffer{};
std::vector<float> Renderer::zBuffer{};
std::vector<Renderer::MeshCommand> Renderer::meshCommands{};
std::vector<Renderer::TextCommand> Renderer::textCommands{};
