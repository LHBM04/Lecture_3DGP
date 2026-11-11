#if UNITY_EDITOR
using System.Collections.Generic;
using System.IO;

using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Rendering;

namespace TUK.Editor
{
    internal struct CubemapFaceData
    {
        internal CubemapFace Face;
        internal byte[] Pixels;
    }

    internal struct CubemapData
    {
        internal string Name;
        internal int Width;
        internal int Height;
        internal bool IsSRGB;
        internal List<CubemapFaceData> Faces;
    }

    internal static class TUKCubemapExporter
    {
        internal static readonly string DirectoryPath = "Resources/Cubemaps/";
        internal static readonly string FileExtension = ".png";

        private static readonly CubemapFace[] FACES =
        {
            CubemapFace.PositiveX,
            CubemapFace.NegativeX,
            CubemapFace.PositiveY,
            CubemapFace.NegativeY,
            CubemapFace.PositiveZ,
            CubemapFace.NegativeZ
        };

        internal static void Save(string directory, CubemapData data)
        {
            Directory.CreateDirectory(directory);

            foreach (CubemapFaceData face in data.Faces)
            {
                string fileName = $"{data.Name}_{face.Face}{FileExtension}";
                string filePath = Path.GetFullPath(Path.Combine(directory, fileName));

                File.WriteAllBytes(filePath, face.Pixels);
                Debug.Log($"[TUK] 큐브맵 면 저장: {filePath} ({face.Pixels.Length:N0} bytes)");
            }
        }

        internal static CubemapData GetMetaData(Cubemap current)
        {
            if (!current)
            {
                return default;
            }

            if (!SystemInfo.supportsAsyncGPUReadback)
            {
                Debug.LogError("현재 그래픽 장치에서 큐브맵 GPU 읽기를 지원하지 않습니다.");
                return default;
            }

            bool isSRGB = GraphicsFormatUtility.IsSRGBFormat(current.graphicsFormat);
            GraphicsFormat format = isSRGB
                ? GraphicsFormat.R8G8B8A8_SRGB
                : GraphicsFormat.R8G8B8A8_UNorm;

            var request = AsyncGPUReadback.Request(current, 0, format);
            request.WaitForCompletion();

            if (request.hasError || request.layerCount != FACES.Length)
            {
                Debug.LogError($"'{current.name}' 큐브맵의 6면을 읽지 못했습니다.");
                return default;
            }

            CubemapData data = new();

            data.Name = TUKBinaryWriter.GetName(current);

            data.Width = request.width;
            data.Height = request.height;
            data.IsSRGB = isSRGB;

            data.Faces = new();

            foreach (CubemapFace face in FACES)
            {
                var pixels = request.GetData<byte>((int)face);
                byte[] image = ImageConversion.EncodeArrayToPNG(
                    pixels.ToArray(), format, (uint)request.width, (uint)request.height, 0);

                if (image == null || image.Length == 0)
                {
                    Debug.LogError($"'{current.name}'의 {face} 면을 PNG로 인코딩하지 못했습니다.");
                    return default;
                }

                CubemapFaceData faceData = new();

                faceData.Face = face;
                faceData.Pixels = image;

                data.Faces.Add(faceData);
            }

            return data;
        }
    }
}
#endif
