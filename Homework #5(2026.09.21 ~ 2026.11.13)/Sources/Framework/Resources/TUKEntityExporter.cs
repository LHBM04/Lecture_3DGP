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
    private const string VERTEX_SHADER = "Shaders/Standard.vert.cso";
    private const string PIXEL_SHADER = "Shaders/Standard.frag.cso";
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
        public string baseColorTexture;
        public string vertexShader;
        public string pixelShader;
    }

    [Serializable]
    private sealed class MeshData
    {
        [NonSerialized] public string path;
        public Vector3[] positions;
        public Vector3[] normals;
        public int[] indices;
    }

    private sealed class ExportData
    {
        public List<MeshData> meshes = new List<MeshData>();
        public List<MaterialData> materials = new List<MaterialData>();
        public Dictionary<Mesh, string[]> meshPaths =
            new Dictionary<Mesh, string[]>();
        public Dictionary<Material, string> materialPaths =
            new Dictionary<Material, string>();
    }

    [MenuItem("Tools/TUK Model Exporter")]
    private static void ExportSelectedModel()
    {
        GameObject selected = Selection.activeGameObject;
        if (selected == null)
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

        try
        {
            var export = new ExportData();
            NodeData root = CollectNode(selected.transform, export);

            if (export.meshes.Count == 0)
            {
                Debug.LogError("선택한 계층에 내보낼 정적 메시가 없습니다.");
                return;
            }

            string folderName = "TUKExport-" + Guid.NewGuid().ToString("N");
            string output = Path.Combine(parentFolder, folderName);
            WriteExport(output, root, export);
            Debug.Log($"TUK 모델 내보내기 완료: {output}");
        }
        catch (Exception exception) when (
            exception is IOException || exception is UnauthorizedAccessException ||
            exception is InvalidOperationException)
        {
            Debug.LogError($"TUK 모델 내보내기 실패: {exception.Message}");
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

        string[] meshPaths = CollectMeshes(mesh, export);
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
                material = CollectMaterial(material, export)
            });
        }
    }

    private static string[] CollectMeshes(Mesh mesh, ExportData export)
    {
        if (export.meshPaths.TryGetValue(mesh, out string[] existingPaths))
        {
            return existingPaths;
        }

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

            string path = $"Meshes/Mesh_{export.meshes.Count:D4}.json";
            paths[slot] = path;
            export.meshes.Add(new MeshData
            {
                path = path,
                positions = positions,
                normals = normals,
                indices = indexBuffer.ToArray()
            });
        }

        export.meshPaths.Add(mesh, paths);
        return paths;
    }

    private static string CollectMaterial(Material material, ExportData export)
    {
        if (export.materialPaths.TryGetValue(material, out string existingPath))
        {
            return existingPath;
        }

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

        Texture texture = null;
        if (material.HasProperty("_BaseMap"))
        {
            texture = material.GetTexture("_BaseMap");
        }
        else if (material.HasProperty("_MainTex"))
        {
            texture = material.GetTexture("_MainTex");
        }

        string path = GetMaterialPath(export.materials.Count);
        export.materials.Add(new MaterialData
        {
            name = material.name,
            baseColor = ToRgba(baseColor),
            emissiveColor = ToRgba(emission),
            metallic = Mathf.Clamp01(metallic),
            roughness = 1f - Mathf.Clamp01(smoothness),
            baseColorTexture = texture != null ? AssetDatabase.GetAssetPath(texture) : "",
            vertexShader = VERTEX_SHADER,
            pixelShader = PIXEL_SHADER
        });
        export.materialPaths.Add(material, path);
        return path;
    }

    private static string GetMaterialPath(int index)
    {
        return $"Materials/Material_{index:D4}.json";
    }

    private static float[] ToRgba(Color color)
    {
        return new[] { color.r, color.g, color.b, color.a };
    }

    // 수집을 마친 뒤 의존 파일을 먼저 쓰고 모델 JSON을 마지막에 기록합니다.
    private static void WriteExport(string output, NodeData root, ExportData export)
    {
        Directory.CreateDirectory(Path.Combine(output, "Meshes"));
        Directory.CreateDirectory(Path.Combine(output, "Materials"));
        Directory.CreateDirectory(Path.Combine(output, "Models"));

        foreach (MeshData mesh in export.meshes)
        {
            WriteJson(Path.Combine(output, mesh.path), mesh);
        }

        for (int index = 0; index < export.materials.Count; index++)
        {
            WriteJson(Path.Combine(output, GetMaterialPath(index)), export.materials[index]);
        }

        WriteModelJson(Path.Combine(output, "Models", "Model.json"), root);
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