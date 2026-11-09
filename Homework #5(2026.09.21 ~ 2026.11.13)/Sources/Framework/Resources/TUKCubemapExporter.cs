using System;
using System.IO;
using System.Linq;

using UnityEditor;
using UnityEngine;
using UnityEngine.Experimental.Rendering;
using UnityEngine.Rendering;

public class TUKCubemapExporter : EditorWindow
{
    private static readonly CubemapFace[] FACES =
    {
        CubemapFace.PositiveX,
        CubemapFace.NegativeX,
        CubemapFace.PositiveY,
        CubemapFace.NegativeY,
        CubemapFace.PositiveZ,
        CubemapFace.NegativeZ
    };

    [MenuItem("Tools/TUK/Export Cubemap")]
    private static void Export()
    {
        Cubemap[] selection = Selection.objects.OfType<Cubemap>().ToArray();
        if (selection.Length == 0)
        {
            Debug.LogError("내보낼 큐브맵 에셋을 선택하세요.");
            return;
        }

        foreach (Cubemap cubemap in selection)
        {
            ExportCubemap(cubemap);
        }

        AssetDatabase.Refresh();
    }

    private static void ExportCubemap(Cubemap cubemap)
    {
        try
        {
            string assetName = GetSafeName(cubemap.name);
            string output = GetAvailableFolder(Application.dataPath, assetName);
            byte[][] faces = EncodeFaces(cubemap);

            Directory.CreateDirectory(output);
            for (int index = 0; index < FACES.Length; index++)
            {
                string fileName = $"{assetName}_{FACES[index]}.png";
                File.WriteAllBytes(Path.Combine(output, fileName), faces[index]);
            }

            Debug.Log($"TUK 큐브맵 내보내기 완료: {output}");
        }
        catch (Exception exception) when (
            exception is IOException || exception is UnauthorizedAccessException ||
            exception is InvalidOperationException)
        {
            Debug.LogError($"TUK 큐브맵 '{cubemap.name}' 내보내기 실패: {exception.Message}");
        }
    }

    private static byte[][] EncodeFaces(Cubemap cubemap)
    {
        if (!SystemInfo.supportsAsyncGPUReadback)
        {
            throw new InvalidOperationException("현재 그래픽 장치에서 큐브맵 GPU 읽기를 지원하지 않습니다.");
        }

        GraphicsFormat format = GraphicsFormatUtility.IsSRGBFormat(cubemap.graphicsFormat)
            ? GraphicsFormat.R8G8B8A8_SRGB
            : GraphicsFormat.R8G8B8A8_UNorm;
        var request = AsyncGPUReadback.Request(cubemap, 0, format);
        request.WaitForCompletion();
        if (request.hasError || request.layerCount != FACES.Length)
        {
            throw new InvalidOperationException($"'{cubemap.name}' 큐브맵의 6면을 읽지 못했습니다.");
        }

        var images = new byte[FACES.Length][];
        for (int index = 0; index < FACES.Length; index++)
        {
            var pixels = request.GetData<byte>((int)FACES[index]);
            images[index] = ImageConversion.EncodeArrayToPNG(
                pixels.ToArray(), format, (uint)request.width, (uint)request.height, 0);
            if (images[index] == null || images[index].Length == 0)
            {
                throw new InvalidOperationException($"'{cubemap.name}'의 {FACES[index]} 면을 PNG로 인코딩하지 못했습니다.");
            }
        }

        return images;
    }

    private static string GetSafeName(string value)
    {
        string name = string.Join("_", value.Split(Path.GetInvalidFileNameChars())).Trim().TrimEnd(' ', '.');
        if (string.IsNullOrEmpty(name))
        {
            return "Unnamed";
        }

        string stem = name.Split('.')[0].TrimEnd().ToUpperInvariant();
        bool numberedDevice = stem.Length == 4 &&
            (stem.StartsWith("COM", StringComparison.Ordinal) || stem.StartsWith("LPT", StringComparison.Ordinal)) &&
            stem[3] >= '0' && stem[3] <= '9';
        return numberedDevice || stem == "CON" || stem == "PRN" || stem == "AUX" || stem == "NUL"
            ? "_" + name
            : name;
    }

    private static string GetAvailableFolder(string parentFolder, string assetName)
    {
        string path = Path.Combine(parentFolder, assetName);
        for (int suffix = 2; Directory.Exists(path) || File.Exists(path); suffix++)
        {
            path = Path.Combine(parentFolder, $"{assetName}_{suffix}");
        }
        return path;
    }
}
