using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using Unity.Collections;
using UnityEditor;
using UnityEngine;
using UnityEngine.Rendering;

public static class TUKEntityExporter
{
    private const string VERTEX_SHADER = "../../Shaders/Standard.vert.cso";
    private const string PIXEL_SHADER = "../../Shaders/Standard.frag.cso";
    private static readonly UTF8Encoding UTF8 = new UTF8Encoding(false, true);

    [Serializable]
    private sealed class NodeData
    {
        public string name;
        [NonSerialized] public List<NodeData> children = new List<NodeData>();
        public float[] localPosition;
        public float[] localRotation;
        public float[] localScale;
        public List<RenderPartData> renderParts = new List<RenderPartData>();
    }

    [Serializable]
    private sealed class RenderPartData
    {
        public string mesh;
        public string material;
    }

    [Serializable]
    private sealed class MaterialData
    {
        public string name;
        public float[] baseColor;
        public float[] emissiveColor;
        public float metallic;
        public float roughness;
        [NonSerialized] public string path;
        public string baseColorTexture;
        public string normalTexture;
        public string metallicTexture;
        public string roughnessTexture;
        public string occlusionTexture;
        public string emissiveTexture;
        public string heightTexture;
        public string specularTexture;
        public string maskTexture;
        public string vertexShader;
        public string pixelShader;
    }

    [Serializable]
    private sealed class MeshData
    {
        [NonSerialized] public string path;
        public Vector3[] positions;
        public Vector3[] normals;
        public Vector2[] uv0;
        public int[] indices;
    }

    private sealed class ExportData
    {
        public List<MeshData> meshes = new List<MeshData>();
        public List<MaterialData> materials = new List<MaterialData>();
        public HashSet<string> partNames = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
        public Dictionary<Texture, string> texturePaths = new Dictionary<Texture, string>();
        public Dictionary<string, string> textureSources =
            new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
    }

    [MenuItem("Tools/TUK/Export Model")]
    private static void ExportSelectedModels()
    {
        GameObject[] selection = Selection.gameObjects;
        if (selection.Length == 0)
        {
            Debug.LogError("Hierarchy에서 내보낼 루트 오브젝트를 선택하세요.");
            return;
        }

        string parentFolder = EditorUtility.SaveFolderPanel(
            "TUK 모델 출력 폴더", "", "");
        if (string.IsNullOrEmpty(parentFolder))
        {
            return;
        }

        var selectedObjects = new HashSet<GameObject>(selection);
        foreach (GameObject selected in selection)
        {
            if (HasSelectedAncestor(selected.transform, selectedObjects))
            {
                continue;
            }

            ExportModel(selected, parentFolder);
        }
    }

    private static bool HasSelectedAncestor(Transform target, HashSet<GameObject> selectedObjects)
    {
        for (Transform parent = target.parent; parent != null; parent = parent.parent)
        {
            if (selectedObjects.Contains(parent.gameObject))
            {
                return true;
            }
        }

        return false;
    }

    private static void ExportModel(GameObject selected, string parentFolder)
    {
        try
        {
            var export = new ExportData();
            NodeData root = CollectNode(selected.transform, export);

            if (export.meshes.Count == 0)
            {
                Debug.LogError($"'{selected.name}' 계층에 내보낼 정적 메시가 없습니다.");
                return;
            }

            string assetName = GetSafeName(root.name);
            string output = GetAvailableFolder(parentFolder, assetName);
            WriteExport(output, assetName, root, export);
            Debug.Log($"TUK 모델 내보내기 완료: {output}");
        }
        catch (Exception exception) when (
            exception is IOException || exception is UnauthorizedAccessException ||
            exception is InvalidOperationException)
        {
            Debug.LogError($"TUK 모델 '{selected.name}' 내보내기 실패: {exception.Message}");
        }
    }

    private static NodeData CollectNode(Transform target, ExportData export)
    {
        Vector3 position = target.localPosition;
        Quaternion rotation = target.localRotation;
        Vector3 scale = target.localScale;
        var node = new NodeData
        {
            name = target.name,
            localPosition = new[] { position.x, position.y, position.z },
            localRotation = new[] { rotation.x, rotation.y, rotation.z, rotation.w },
            localScale = new[] { scale.x, scale.y, scale.z }
        };

        CollectRenderParts(target, node, export);

        foreach (Transform child in target)
        {
            node.children.Add(CollectNode(child, export));
        }

        return node;
    }

    private static void CollectRenderParts(Transform target, NodeData node, ExportData export)
    {
        if (target.TryGetComponent(out SkinnedMeshRenderer _))
        {
            throw new InvalidOperationException(
                $"'{target.name}'의 스킨드 메시는 이 exporter에서 지원하지 않습니다.");
        }

        if (!target.TryGetComponent(out MeshFilter filter) ||
            !target.TryGetComponent(out MeshRenderer renderer))
        {
            return;
        }

        Mesh mesh = filter.sharedMesh;
        if (mesh == null || !mesh.isReadable)
        {
            throw new InvalidOperationException(
                $"'{target.name}'의 메시가 없거나 읽기 설정이 꺼져 있습니다.");
        }

        Material[] materials = renderer.sharedMaterials;
        if (materials.Length != mesh.subMeshCount)
        {
            throw new InvalidOperationException(
                $"'{target.name}'의 서브메시 수와 머티리얼 슬롯 수가 다릅니다.");
        }

        string objectName = GetSafeName(target.name);
        var partNames = new string[materials.Length];
        for (int slot = 0; slot < partNames.Length; slot++)
        {
            string partName = partNames.Length == 1 ? objectName : $"{objectName}_{slot + 1}";
            partNames[slot] = ReserveName(export.partNames, partName);
        }

        string[] meshPaths = CollectMeshes(mesh, partNames, export);
        for (int slot = 0; slot < meshPaths.Length; slot++)
        {
            Material material = materials[slot];
            if (material == null)
            {
                throw new InvalidOperationException(
                    $"'{target.name}'의 머티리얼 슬롯 {slot}이 비어 있습니다.");
            }

            node.renderParts.Add(new RenderPartData
            {
                mesh = meshPaths[slot],
                material = CollectMaterial(material, partNames[slot], export)
            });
        }
    }

    private static string[] CollectMeshes(Mesh mesh, string[] partNames, ExportData export)
    {
        using var snapshot = Mesh.AcquireReadOnlyMeshData(mesh);
        Mesh.MeshData data = snapshot[0];
        if (data.vertexCount == 0 || !data.HasVertexAttribute(VertexAttribute.Normal))
        {
            throw new InvalidOperationException($"'{mesh.name}'에 정점 또는 노멀이 없습니다.");
        }

        using var vertexBuffer = new NativeArray<Vector3>(
            data.vertexCount, Allocator.Temp, NativeArrayOptions.UninitializedMemory);
        using var normalBuffer = new NativeArray<Vector3>(
            data.vertexCount, Allocator.Temp, NativeArrayOptions.UninitializedMemory);
        data.GetVertices(vertexBuffer);
        data.GetNormals(normalBuffer);

        // 저장 단계까지 살아 있어야 하므로 관리 배열에 한 번만 복사합니다.
        Vector3[] positions = vertexBuffer.ToArray();
        Vector3[] normals = normalBuffer.ToArray();
        Vector2[] uv0 = Array.Empty<Vector2>();
        if (data.HasVertexAttribute(VertexAttribute.TexCoord0))
        {
            using var uvBuffer = new NativeArray<Vector2>(
                data.vertexCount, Allocator.Temp, NativeArrayOptions.UninitializedMemory);
            data.GetUVs(0, uvBuffer);
            uv0 = uvBuffer.ToArray();
        }

        var paths = new string[data.subMeshCount];
        for (int slot = 0; slot < data.subMeshCount; slot++)
        {
            SubMeshDescriptor subMesh = data.GetSubMesh(slot);
            if (subMesh.topology != MeshTopology.Triangles ||
                subMesh.indexCount == 0 || subMesh.indexCount % 3 != 0)
            {
                throw new InvalidOperationException(
                    $"'{mesh.name}'의 서브메시 {slot}에 올바른 삼각형 인덱스가 없습니다.");
            }

            using var indexBuffer = new NativeArray<int>(
                subMesh.indexCount, Allocator.Temp, NativeArrayOptions.UninitializedMemory);
            data.GetIndices(indexBuffer, slot, applyBaseVertex: true);

            string path = $"Meshes/{partNames[slot]}.json";
            paths[slot] = path;
            export.meshes.Add(new MeshData
            {
                path = path,
                positions = positions,
                normals = normals,
                uv0 = uv0,
                indices = indexBuffer.ToArray()
            });
        }

        return paths;
    }

    private static string CollectMaterial(Material material, string partName, ExportData export)
    {
        Color baseColor = Color.white;
        if (material.HasProperty("_BaseColor"))
        {
            baseColor = material.GetColor("_BaseColor");
        }
        else if (material.HasProperty("_Color"))
        {
            baseColor = material.GetColor("_Color");
        }

        Color emission = material.HasProperty("_EmissionColor")
            ? material.GetColor("_EmissionColor")
            : Color.black;
        float metallic = material.HasProperty("_Metallic") ? material.GetFloat("_Metallic") : 0f;
        float smoothness = 0.5f;
        if (material.HasProperty("_Smoothness"))
        {
            smoothness = material.GetFloat("_Smoothness");
        }
        else if (material.HasProperty("_Glossiness"))
        {
            smoothness = material.GetFloat("_Glossiness");
        }

        string path = $"Materials/{partName}.json";
        export.materials.Add(new MaterialData
        {
            path = path,
            name = partName,
            baseColor = ToRgba(baseColor),
            emissiveColor = ToRgba(emission),
            metallic = Mathf.Clamp01(metallic),
            roughness = 1f - Mathf.Clamp01(smoothness),
            baseColorTexture = CollectTexture(material, partName, "BaseColor", export, "_BaseMap", "_BaseColorMap", "_MainTex"),
            normalTexture = CollectTexture(material, partName, "Normal", export, "_BumpMap", "_NormalMap"),
            metallicTexture = CollectTexture(material, partName, "Metallic", export, "_MetallicGlossMap", "_MetallicMap"),
            roughnessTexture = CollectTexture(material, partName, "Roughness", export, "_RoughnessMap"),
            occlusionTexture = CollectTexture(material, partName, "Occlusion", export, "_OcclusionMap", "_AmbientOcclusionMap"),
            emissiveTexture = CollectTexture(material, partName, "Emissive", export, "_EmissionMap", "_EmissiveColorMap"),
            heightTexture = CollectTexture(material, partName, "Height", export, "_ParallaxMap", "_HeightMap"),
            specularTexture = CollectTexture(material, partName, "Specular", export, "_SpecGlossMap", "_SpecularColorMap"),
            maskTexture = CollectTexture(material, partName, "Mask", export, "_MaskMap"),
            vertexShader = VERTEX_SHADER,
            pixelShader = PIXEL_SHADER
        });
        return path;
    }

    private static string CollectTexture(
        Material material, string partName, string mapName, ExportData export, params string[] properties)
    {
        foreach (string property in properties)
        {
            if (!material.HasProperty(property))
            {
                continue;
            }

            Texture texture = material.GetTexture(property);
            if (texture == null)
            {
                continue;
            }

            if (export.texturePaths.TryGetValue(texture, out string existingPath))
            {
                return existingPath;
            }

            string assetPath = AssetDatabase.GetAssetPath(texture);
            if (!(texture is Texture2D) || string.IsNullOrEmpty(assetPath) ||
                !(AssetImporter.GetAtPath(assetPath) is TextureImporter))
            {
                throw new InvalidOperationException(
                    $"'{material.name}'의 '{property}' 맵은 원본 이미지 파일로 추출한 뒤 내보내세요.");
            }

            var package = UnityEditor.PackageManager.PackageInfo.FindForAssetPath(assetPath);
            string source = package == null
                ? Path.GetFullPath(Path.Combine(Application.dataPath, "..", assetPath))
                : Path.Combine(package.resolvedPath, assetPath.Substring(package.assetPath.Length).TrimStart('/'));
            if (!File.Exists(source))
            {
                throw new FileNotFoundException($"'{material.name}'의 '{property}' 원본 파일을 찾지 못했습니다.", source);
            }

            string extension = Path.GetExtension(source);
            string path = $"Textures/{partName}_{mapName}{extension}";
            for (int suffix = 2; export.textureSources.ContainsKey(path); suffix++)
            {
                path = $"Textures/{partName}_{mapName}_{suffix}{extension}";
            }

            export.texturePaths.Add(texture, path);
            export.textureSources.Add(path, source);
            return path;
        }

        return "";
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

    private static string ReserveName(HashSet<string> names, string baseName)
    {
        string name = baseName;
        for (int suffix = 2; !names.Add(name); suffix++)
        {
            name = $"{baseName}_{suffix}";
        }
        return name;
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

    private static float[] ToRgba(Color color)
    {
        return new[] { color.r, color.g, color.b, color.a };
    }

    // 수집을 마친 뒤 의존 파일을 먼저 쓰고 모델 JSON을 마지막에 기록합니다.
    private static void WriteExport(string output, string assetName, NodeData root, ExportData export)
    {
        Directory.CreateDirectory(Path.Combine(output, "Meshes"));
        Directory.CreateDirectory(Path.Combine(output, "Materials"));
        Directory.CreateDirectory(Path.Combine(output, "Models"));
        Directory.CreateDirectory(Path.Combine(output, "Textures"));

        foreach (var texture in export.textureSources)
        {
            File.Copy(texture.Value, Path.Combine(output, texture.Key));
        }

        foreach (MeshData mesh in export.meshes)
        {
            WriteJson(Path.Combine(output, mesh.path), mesh);
        }

        foreach (MaterialData material in export.materials)
        {
            WriteJson(Path.Combine(output, material.path), material);
        }

        WriteModelJson(Path.Combine(output, "Models", $"{assetName}.json"), root);
    }

    private static void WriteModelJson(string path, NodeData root)
    {
        using var writer = new StreamWriter(path, false, UTF8);
        WriteNodeJson(writer, root, "");
        writer.WriteLine();
    }

    private static void WriteNodeJson(TextWriter writer, NodeData node, string indent)
    {
        WriteObjectFields(writer, node, indent);
        writer.WriteLine(",");
        writer.Write(indent);
        writer.Write("  \"children\": [");

        string childIndent = indent + "    ";
        for (int index = 0; index < node.children.Count; index++)
        {
            if (index > 0)
            {
                writer.Write(",");
            }

            writer.WriteLine();
            writer.Write(childIndent);
            WriteNodeJson(writer, node.children[index], childIndent);
        }

        if (node.children.Count > 0)
        {
            writer.WriteLine();
            writer.Write(indent);
            writer.Write("  ");
        }

        writer.WriteLine("]");
        writer.Write(indent);
        writer.Write("}");
    }

    // Unity의 재귀 직렬화 제한을 피하기 위해 현재 객체의 필드만 기록합니다.
    private static void WriteObjectFields(TextWriter writer, object data, string indent)
    {
        string json = JsonUtility.ToJson(data, prettyPrint: true);
        string fields = json.Substring(0, json.LastIndexOf('}')).TrimEnd();
        writer.Write(fields.Replace("\n", "\n" + indent));
    }

    private static void WriteJson(string path, object data)
    {
        File.WriteAllText(path, JsonUtility.ToJson(data, prettyPrint: true), UTF8);
    }
}