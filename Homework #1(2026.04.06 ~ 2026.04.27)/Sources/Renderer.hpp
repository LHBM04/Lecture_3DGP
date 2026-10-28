#pragma once

#include "ColorRGBA.hpp"
#include "Matrix4x4.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

class Camera;
class Mesh;
class Object;

class Renderer final
{
	STATIC_CLASS(Renderer)
public:
	struct Options final
	{
		HWND hWnd;
		int width;
		int height;
		ColorRGBA clearColor;
	};

	enum class PrimitiveType : unsigned int
	{
		Triangles,
		Lines,
		Points
	};

	enum class PolygonMode : unsigned int
	{
		Wire,
		Solid
	};

	struct RasterVertex final
	{
		int x;
		int y;
		float z;
		ColorRGBA color;
	};

	struct MeshCommand final
	{
		Mesh* mesh;
		Camera* camera;
		Object* object;
		ColorRGBA color;
	};

	struct TextCommand final
	{
		std::wstring text;
		RECT rect;
		ColorRGBA textColor;
		ColorRGBA shadowColor;
		bool drawShadow;
		POINT shadowOffset;
		UINT format;
		HFONT font;
	};

	static int GetWidth();
	static void SetWidth(int width_);

	static int GetHeight();
	static void SetHeight(int height_);

	static void Resize(int width_, int height_);

	static void SetPolygonMode(PolygonMode polygonMode_);
	[[nodiscard]] static PolygonMode GetPolygonMode();

	static void SetCamera(Camera* camera_);
	[[nodiscard]] static Camera* GetCamera();

	static void SetObject(Object* object_);
	[[nodiscard]] static Object* GetObject();

	static bool Initialize(const Options& options_);
	static void Terminate();

	static void Reset();
	static void QueueMesh(Mesh* mesh_, const ColorRGBA& color_);
	static void QueueText(const TextCommand& textCommand_);
	static void Present();

private:
	static constexpr std::size_t BYTES_PER_PIXEL = 4;
	static constexpr std::size_t BITS_PER_PIXEL = BYTES_PER_PIXEL * 8;
	static constexpr float CLIP_Z_NEAR = 0.0f;
	static constexpr float CLIP_Z_FAR = 1.0f;

	static Options options;

	static PolygonMode polygonMode;

	static Camera* camera;
	static Object* object;

	static HDC windowDC;
	static HDC backBufferDC;
	static HBITMAP backBufferBitmap;
	static HGDIOBJ previousBackBufferBitmap;

	static std::vector<unsigned int> colorBuffer;
	static std::vector<float> zBuffer;
	static std::vector<MeshCommand> meshCommands;
	static std::vector<TextCommand> textCommands;

	static unsigned int ToPackedColor(const ColorRGBA& color_);
	static float EdgeFunction(float ax_, float ay_, float bx_, float by_, float px_, float py_);
	static bool RecreateBackBuffer();
	static void RasterizeMeshCommand(const MeshCommand& meshCommand_);
	static void DrawTrianglesByArrays(const std::vector<RasterVertex>& vertices_);
	static void DrawTrianglesByElements(const std::vector<RasterVertex>& vertices_, const std::vector<std::uint32_t>& indices_);
	static void DrawLineScreen(int x0_, int y0_, float z0_, int x1_, int y1_, float z1_, unsigned int color_);
	static void DrawTriangleFilledScreen(
		int x0_, int y0_, float z0_,
		int x1_, int y1_, float z1_,
		int x2_, int y2_, float z2_,
		unsigned int color_);
	static void DrawPixelScreen(int x_, int y_, float z_, unsigned int color_);
};
