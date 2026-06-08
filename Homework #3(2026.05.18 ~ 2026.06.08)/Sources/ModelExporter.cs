using System.Collections.Generic;
using System.IO;
using System.Text;
using UnityEngine;

public class ModelExporter : MonoBehaviour
{
    [SerializeField] private string outputFolderName = "Homework3Export";
    [SerializeField] private string shaderPath = "Resources/Shaders/Standard.hlsl";
    [SerializeField] private bool exportOnStart = true;
    [SerializeField] private float animationSampleFps = 30f;

    private sealed class RendererResourceRecord
    {
        public string MeshResourceName;
        public string MaterialResourceName;
    }

    private sealed class ModelNodeRecord
    {
        public int Index;
        public int ParentIndex;
        public string Name;
        public Vector3 LocalPosition;
        public Quaternion LocalRotation;
        public Vector3 LocalScale;
        public readonly List<RendererResourceRecord> Renderers = new List<RendererResourceRecord>();
    }

    private sealed class KeyframeRecord
    {
        public float Time;
        public Vector3 Position;
        public Quaternion Rotation;
        public Vector3 Scale;
    }

    private sealed class AnimationTrackRecord
    {
        public string TargetNodeName;
        public readonly List<KeyframeRecord> Keyframes = new List<KeyframeRecord>();
    }

    private sealed class AnimationClipRecord
    {
        public string ClipName;
        public float Duration;
        public float FrameRate;
        public readonly List<AnimationTrackRecord> Tracks = new List<AnimationTrackRecord>();
    }

    [ContextMenu("Export Homework3 Resources")]
    private void ExportHomework3Resources()
    {
        string exportRoot = Path.Combine(Application.dataPath, "..", outputFolderName);
        string meshDirectory = Path.Combine(exportRoot, "Meshes");
        string materialDirectory = Path.Combine(exportRoot, "Materials");
        string modelDirectory = Path.Combine(exportRoot, "Models");
        string animationDirectory = Path.Combine(exportRoot, "Animations");

        Directory.CreateDirectory(meshDirectory);
        Directory.CreateDirectory(materialDirectory);
        Directory.CreateDirectory(modelDirectory);
        Directory.CreateDirectory(animationDirectory);

        HashSet<string> exportedMaterialPaths = new HashSet<string>();
        List<ModelNodeRecord> modelNodes = new List<ModelNodeRecord>();
        int exportedMeshCount = 0;
        int exportedMaterialCount = 0;

        ExportFrameRecursive(
            transform,
            -1,
            meshDirectory,
            materialDirectory,
            exportedMaterialPaths,
            modelNodes,
            ref exportedMeshCount,
            ref exportedMaterialCount);

        string modelPath = Path.Combine(modelDirectory, SanitizeName(transform.name) + ".bin");
        WriteModelResource(modelPath, transform.name, modelNodes);

        int exportedAnimCount = 0;
        Animator animator = GetComponent<Animator>();
        if (animator != null)
        {
#if UNITY_EDITOR
            var controller = animator.runtimeAnimatorController as UnityEditor.Animations.AnimatorController;
            if (controller != null)
            {
                HashSet<AnimationClip> uniqueClips = new HashSet<AnimationClip>();
                foreach (var layer in controller.layers)
                {
                    GetClipsFromStateGroup(layer.stateMachine, uniqueClips);
                }

                Dictionary<Transform, (Vector3 p, Quaternion r, Vector3 s)> transformBackup = BackupTransforms(transform);

                foreach (AnimationClip clip in uniqueClips)
                {
                    if (clip == null) continue;

                    AnimationClipRecord animRecord = CaptureAnimationClip(gameObject, clip, animationSampleFps);
                    string animPath = Path.Combine(animationDirectory, animRecord.ClipName + ".bin");
                    WriteAnimationResource(animPath, animRecord);
                    exportedAnimCount++;
                }

                RestoreTransforms(transformBackup);
#endif
            }
        }

        Debug.Log(
            $"<color=green><b>[내보내기 완료]</b></color> Homework #3 resource export completed.\n" +
            $"Root: {Path.GetFullPath(exportRoot)}\n" +
            $"Meshes: {exportedMeshCount}, Materials: {exportedMaterialCount}, Models: 1, Animations: {exportedAnimCount}");
    }

    private void Start()
    {
        if (exportOnStart)
        {
            ExportHomework3Resources();
        }
    }

#if UNITY_EDITOR
    private void GetClipsFromStateGroup(UnityEditor.Animations.AnimatorStateMachine stateMachine, HashSet<AnimationClip> clips)
    {
        foreach (var state in stateMachine.states)
        {
            if (state.state.motion is AnimationClip clip)
            {
                clips.Add(clip);
            }
        }
        foreach (var subMachine in stateMachine.stateMachines)
        {
            GetClipsFromStateGroup(subMachine.stateMachine, clips);
        }
    }
#endif
    private AnimationClipRecord CaptureAnimationClip(GameObject rootObject, AnimationClip clip, float fps)
    {
        AnimationClipRecord clipRecord = new AnimationClipRecord
        {
            ClipName = SanitizeName(clip.name),
            Duration = clip.length,
            FrameRate = fps
        };

        Transform[] allTransforms = rootObject.GetComponentsInChildren<Transform>(true);
        foreach (var t in allTransforms)
        {
            clipRecord.Tracks.Add(new AnimationTrackRecord { TargetNodeName = t.name });
        }

        float timeStep = 1f / fps;
        float currentTime = 0f;

        while (currentTime <= clip.length)
        {
            clip.SampleAnimation(rootObject, currentTime);

            for (int i = 0; i < allTransforms.Length; i++)
            {
                Transform t = allTransforms[i];
                clipRecord.Tracks[i].Keyframes.Add(new KeyframeRecord
                {
                    Time = currentTime,
                    Position = t.localPosition,
                    Rotation = t.localRotation,
                    Scale = t.localScale
                });
            }

            currentTime += timeStep;
        }

        return clipRecord;
    }

    private void ExportFrameRecursive(
        Transform current,
        int parentNodeIndex,
        string meshDirectory,
        string materialDirectory,
        HashSet<string> exportedMaterialPaths,
        List<ModelNodeRecord> modelNodes,
        ref int exportedMeshCount,
        ref int exportedMaterialCount)
    {
        ModelNodeRecord node = new ModelNodeRecord
        {
            Index = modelNodes.Count,
            ParentIndex = parentNodeIndex,
            Name = current.name,
            LocalPosition = current.localPosition,
            LocalRotation = current.localRotation,
            LocalScale = current.localScale
        };

        modelNodes.Add(node);

        ExportRendererResources(
            current,
            meshDirectory,
            materialDirectory,
            exportedMaterialPaths,
            node,
            ref exportedMeshCount,
            ref exportedMaterialCount);

        for (int i = 0; i < current.childCount; ++i)
        {
            ExportFrameRecursive(
                current.GetChild(i),
                node.Index,
                meshDirectory,
                materialDirectory,
                exportedMaterialPaths,
                modelNodes,
                ref exportedMeshCount,
                ref exportedMaterialCount);
        }
    }

    private void ExportRendererResources(
        Transform current,
        string meshDirectory,
        string materialDirectory,
        HashSet<string> exportedMaterialPaths,
        ModelNodeRecord node,
        ref int exportedMeshCount,
        ref int exportedMaterialCount)
    {
        Mesh mesh = null;
        Material[] materials = null;

        MeshFilter meshFilter = current.GetComponent<MeshFilter>();
        MeshRenderer meshRenderer = current.GetComponent<MeshRenderer>();
        if (meshFilter != null && meshRenderer != null)
        {
            mesh = meshFilter.sharedMesh;
            materials = meshRenderer.sharedMaterials;
        }
        else
        {
            SkinnedMeshRenderer skinnedMeshRenderer = current.GetComponent<SkinnedMeshRenderer>();
            if (skinnedMeshRenderer != null)
            {
                mesh = skinnedMeshRenderer.sharedMesh;
                materials = skinnedMeshRenderer.sharedMaterials;
            }
        }

        if (mesh == null) return;

        if (mesh.vertexCount <= 0)
        {
            Debug.LogWarning($"Skip export. Mesh has no vertices: object={current.name}, mesh={mesh.name}");
            return;
        }

        int subMeshCount = Mathf.Max(1, mesh.subMeshCount);
        for (int subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
        {
            string meshResourceName = BuildMeshResourceName(current, mesh, subMeshIndex, subMeshCount);
            string meshPath = Path.Combine(meshDirectory, meshResourceName + ".bin");
            WriteMeshResource(meshPath, mesh, subMeshIndex);
            ++exportedMeshCount;

            Material material = ResolveMaterial(materials, subMeshIndex);
            string materialResourceName = BuildMaterialResourceName(current, mesh, material, subMeshIndex, subMeshCount);
            string materialPath = Path.Combine(materialDirectory, materialResourceName + ".bin");
            if (!exportedMaterialPaths.Contains(materialPath))
            {
                WriteMaterialResource(materialPath, material);
                exportedMaterialPaths.Add(materialPath);
                ++exportedMaterialCount;
            }

            node.Renderers.Add(new RendererResourceRecord
            {
                MeshResourceName = meshResourceName,
                MaterialResourceName = materialResourceName
            });
        }
    }

    private static Material ResolveMaterial(Material[] materials, int subMeshIndex)
    {
        if (materials == null || materials.Length == 0) return null;
        if (subMeshIndex < materials.Length) return materials[subMeshIndex];
        return materials[0];
    }

    private static string BuildMeshResourceName(Transform current, Mesh mesh, int subMeshIndex, int subMeshCount)
    {
        string name = $"{SanitizeName(current.name)}_{SanitizeName(mesh.name)}";
        if (subMeshCount > 1) name += $"_SM{subMeshIndex}";
        return name;
    }

    private static string BuildMaterialResourceName(Transform current, Mesh mesh, Material material, int subMeshIndex, int subMeshCount)
    {
        string materialName = material != null ? material.name : "Default";
        string name = $"{SanitizeName(current.name)}_{SanitizeName(mesh.name)}_{SanitizeName(materialName)}";
        if (subMeshCount > 1) name += $"_SM{subMeshIndex}";
        return name;
    }

    private static string SanitizeName(string value)
    {
        if (string.IsNullOrWhiteSpace(value)) return "Unnamed";
        StringBuilder builder = new StringBuilder(value.Length);
        foreach (char c in value)
        {
            if (char.IsLetterOrDigit(c) || c == '_') builder.Append(c);
            else if (char.IsWhiteSpace(c) || c == '-') builder.Append('_');
        }
        return builder.Length > 0 ? builder.ToString() : "Unnamed";
    }

    private void WriteMeshResource(string path, Mesh mesh, int subMeshIndex)
    {
        using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
        {
            WriteTag(writer, "<Mesh>:");
            writer.Write(mesh.vertexCount);
            WriteString(writer, mesh.name);
            WriteTag(writer, "<Bounds>:");
            WriteVector3(writer, mesh.bounds.center);
            WriteVector3(writer, mesh.bounds.extents);
            WriteVector3Array(writer, "<Positions>:", mesh.vertices);
            WriteColorArray(writer, "<Colors>:", mesh.colors);
            WriteVector3Array(writer, "<Normals>:", mesh.normals);
            WriteVector2Array(writer, "<TextureCoords>:", mesh.uv);
            WriteIndices(writer, "<Indices>:", mesh.GetTriangles(subMeshIndex));
            WriteTag(writer, "</Mesh>");
        }
    }

    private void WriteMaterialResource(string path, Material material)
    {
        using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
        {
            WriteTag(writer, "<Material>:");
            if (material != null)
            {
                if (material.HasProperty("_Color")) { WriteTag(writer, "<AlbedoColor>:"); WriteColor(writer, material.GetColor("_Color")); }
                if (material.HasProperty("_EmissionColor")) { WriteTag(writer, "<EmissiveColor>:"); WriteColor(writer, material.GetColor("_EmissionColor")); }
                if (material.HasProperty("_SpecColor")) { WriteTag(writer, "<SpecularColor>:"); WriteColor(writer, material.GetColor("_SpecColor")); }
                if (material.HasProperty("_Glossiness")) { WriteTag(writer, "<Glossiness>:"); writer.Write(material.GetFloat("_Glossiness")); }
                if (material.HasProperty("_Smoothness")) { WriteTag(writer, "<Smoothness>:"); writer.Write(material.GetFloat("_Smoothness")); }
                if (material.HasProperty("_Metallic")) { WriteTag(writer, "<Metallic>:"); writer.Write(material.GetFloat("_Metallic")); }
                if (material.HasProperty("_SpecularHighlights")) { WriteTag(writer, "<SpecularHighlight>:"); writer.Write(material.GetFloat("_SpecularHighlights")); }
                if (material.HasProperty("_GlossyReflections")) { WriteTag(writer, "<GlossyReflection>:"); writer.Write(material.GetFloat("_GlossyReflections")); }
            }
            WriteTag(writer, "<Shader>:");
            WriteRawShaderPath(writer, shaderPath);
        }
    }

    private void WriteModelResource(string path, string modelName, List<ModelNodeRecord> nodes)
    {
        using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
        {
            WriteTag(writer, "<Model>:");
            WriteString(writer, modelName);
            WriteTag(writer, "<Nodes>:");
            writer.Write(nodes.Count);
            for (int i = 0; i < nodes.Count; ++i)
            {
                ModelNodeRecord node = nodes[i];
                WriteTag(writer, "<Node>:");
                writer.Write(node.Index);
                writer.Write(node.ParentIndex);
                WriteString(writer, node.Name);
                WriteTag(writer, "<LocalPosition>:");
                WriteVector3(writer, node.LocalPosition);
                WriteTag(writer, "<LocalRotation>:");
                WriteQuaternion(writer, node.LocalRotation);
                WriteTag(writer, "<LocalScale>:");
                WriteVector3(writer, node.LocalScale);
                WriteTag(writer, "<Renderers>:");
                writer.Write(node.Renderers.Count);
                for (int r = 0; r < node.Renderers.Count; ++r)
                {
                    WriteTag(writer, "<Renderer>:");
                    WriteString(writer, node.Renderers[r].MeshResourceName);
                    WriteString(writer, node.Renderers[r].MaterialResourceName);
                }
            }
        }
    }

    private void WriteAnimationResource(string path, AnimationClipRecord clip)
    {
        using (BinaryWriter writer = new BinaryWriter(File.Open(path, FileMode.Create)))
        {
            WriteTag(writer, "<Animation>:");
            WriteString(writer, clip.ClipName);
            writer.Write(clip.Duration);
            writer.Write(clip.FrameRate);

            WriteTag(writer, "<Tracks>:");
            writer.Write(clip.Tracks.Count);

            for (int t = 0; t < clip.Tracks.Count; ++t)
            {
                AnimationTrackRecord track = clip.Tracks[t];
                WriteTag(writer, "<Track>:");
                WriteString(writer, track.TargetNodeName);

                WriteTag(writer, "<Keyframes>:");
                writer.Write(track.Keyframes.Count);

                for (int k = 0; k < track.Keyframes.Count; ++k)
                {
                    KeyframeRecord key = track.Keyframes[k];
                    writer.Write(key.Time);
                    WriteVector3(writer, key.Position);
                    WriteQuaternion(writer, key.Rotation);
                    WriteVector3(writer, key.Scale);
                }
            }
            WriteTag(writer, "</Animation>");
        }
    }

    private Dictionary<Transform, (Vector3, Quaternion, Vector3)> BackupTransforms(Transform root)
    {
        var backup = new Dictionary<Transform, (Vector3, Quaternion, Vector3)>();
        foreach (var t in root.GetComponentsInChildren<Transform>(true))
        {
            backup[t] = (t.localPosition, t.localRotation, t.localScale);
        }
        return backup;
    }

    private void RestoreTransforms(Dictionary<Transform, (Vector3 p, Quaternion r, Vector3 s)> backup)
    {
        foreach (var pair in backup)
        {
            if (pair.Key != null)
            {
                pair.Key.localPosition = pair.Value.p;
                pair.Key.localRotation = pair.Value.r;
                pair.Key.localScale = pair.Value.s;
            }
        }
    }

    private static void WriteTag(BinaryWriter writer, string value)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(value);
        writer.Write((byte)bytes.Length);
        writer.Write(bytes);
    }

    private static void WriteString(BinaryWriter writer, string value)
    {
        string safeValue = string.IsNullOrEmpty(value) ? "Unnamed" : value.Replace(" ", "_");
        byte[] bytes = Encoding.UTF8.GetBytes(safeValue);
        writer.Write((byte)bytes.Length);
        writer.Write(bytes);
    }

    private static void WriteRawShaderPath(BinaryWriter writer, string value)
    {
        byte[] bytes = Encoding.UTF8.GetBytes(value);
        writer.Write((byte)'!');
        writer.Write(bytes);
    }

    private static void WriteVector2Array(BinaryWriter writer, string tag, Vector2[] values) { WriteTag(writer, tag); writer.Write(values.Length); for (int i = 0; i < values.Length; ++i) WriteVector2(writer, values[i]); }
    private static void WriteVector3Array(BinaryWriter writer, string tag, Vector3[] values) { WriteTag(writer, tag); writer.Write(values.Length); for (int i = 0; i < values.Length; ++i) WriteVector3(writer, values[i]); }
    private static void WriteColorArray(BinaryWriter writer, string tag, Color[] values) { WriteTag(writer, tag); writer.Write(values.Length); for (int i = 0; i < values.Length; ++i) WriteColor(writer, values[i]); }
    private static void WriteIndices(BinaryWriter writer, string tag, int[] values) { WriteTag(writer, tag); writer.Write(values.Length); for (int i = 0; i < values.Length; ++i) writer.Write(values[i]); }
    private static void WriteVector2(BinaryWriter writer, Vector2 value) { writer.Write(value.x); writer.Write(value.y); }
    private static void WriteVector3(BinaryWriter writer, Vector3 value) { writer.Write(value.x); writer.Write(value.y); writer.Write(value.z); }
    private static void WriteQuaternion(BinaryWriter writer, Quaternion value) { writer.Write(value.x); writer.Write(value.y); writer.Write(value.z); writer.Write(value.w); }
    private static void WriteColor(BinaryWriter writer, Color value) { writer.Write(value.r); writer.Write(value.g); writer.Write(value.b); writer.Write(value.a); }
}