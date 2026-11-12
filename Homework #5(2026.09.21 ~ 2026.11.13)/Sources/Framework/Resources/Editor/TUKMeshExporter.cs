#if UNITY_EDITOR
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal struct MeshData
    {
        internal List<Vector3> Position;
        internal List<Vector3> Normal;
        internal List<Vector2> TexCoord;
        internal List<int> Indicies;
    }

    internal static class TUKMeshExporter
    {
        internal static readonly string DirectoryPath = "Resources/Meshes/";
        internal static readonly string FileExtension = ".tukmesh";

        internal static string GetRelativePath(Mesh mesh, string modelName = null)
        {
            string meshDirectory = modelName;
            if (string.IsNullOrEmpty(meshDirectory))
            {
                string assetPath = AssetDatabase.GetAssetPath(mesh).Replace('\\', '/');
                int meshesIndex = assetPath.LastIndexOf("/Meshes/", StringComparison.OrdinalIgnoreCase);
                if (meshesIndex >= 0)
                {
                    string meshPath = assetPath.Substring(meshesIndex + "/Meshes/".Length);
                    meshDirectory = Path.GetDirectoryName(meshPath)?.Replace('\\', '/') ?? "";
                }

                if (string.IsNullOrEmpty(meshDirectory))
                {
                    meshDirectory = Path.GetFileNameWithoutExtension(assetPath);
                }
                if (string.IsNullOrEmpty(meshDirectory))
                {
                    meshDirectory = TUKBinaryWriter.GetName(mesh);
                }
            }

            return $"{DirectoryPath}{meshDirectory}/{TUKBinaryWriter.GetName(mesh)}{FileExtension}";
        }

        internal static void Save(string path, MeshData data)
        {
            using (TUKBinaryWriter writer = new(path))
            {
                Save(writer, data);
            }
        }

        internal static void Save(TUKBinaryWriter writer, MeshData data)
        {
            writer.Write(data.Position.Count);
            foreach (Vector3 value in data.Position)
            {
                writer.Write(value);
            }

            writer.Write(data.Normal.Count);
            foreach (Vector3 value in data.Normal)
            {
                writer.Write(value);
            }

            writer.Write(data.TexCoord.Count);
            foreach (Vector2 value in data.TexCoord)
            {
                writer.Write(value);
            }

            writer.Write(data.Indicies.Count);
            foreach (int value in data.Indicies)
            {
                writer.Write(value);
            }
        }

        internal static MeshData GetMetaData(Mesh current)
        {
            if (!current.isReadable)
            {
                throw new InvalidOperationException($"{current.name}: Read/Write가 꺼져 있습니다.");
            }

            MeshData meshData = new();
            meshData.Position = new(current.vertices);
            meshData.Normal = new(current.normals);
            meshData.TexCoord = new(current.uv);
            meshData.Indicies = new(current.triangles);
            return meshData;
        }
    }
}
#endif
