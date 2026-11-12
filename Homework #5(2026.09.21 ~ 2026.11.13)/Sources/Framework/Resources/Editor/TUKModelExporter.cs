using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal struct ModelData
    {
        internal string Name;
        internal Vector3 LocalPosition;
        internal Quaternion LocalRotation;
        internal Vector3 LocalScale;
        internal string MeshPath;
        internal string MaterialPath;
        internal int ChildCount;
        internal List<ModelData> Children;
    }

    internal static class TUKModelExporter
    {
        internal static readonly string DirectoryPath = "Resources/Models/";
        internal static readonly string FileExtension = ".tukmodel";

        internal static void Save(TUKBinaryWriter writer, ModelData data)
        {
            writer.Write(data.Name);
            writer.Write(data.LocalPosition);
            writer.Write(data.LocalRotation);
            writer.Write(data.LocalScale);
            writer.Write(data.MeshPath);
            writer.Write(data.MaterialPath);
            writer.Write(data.Children.Count);

            foreach (ModelData child in data.Children)
            {
                Save(writer, child);
            }
        }

        internal static ModelData GetMetaData(Transform current)
        {
            return GetMetaData(current, Environment.CurrentDirectory);
        }

        internal static ModelData GetMetaData(Transform current, string outputRoot)
        {
            return GetMetaData(current, outputRoot, ShaderData.GetDefault());
        }

        internal static ModelData GetMetaData(Transform current, string outputRoot, ShaderData shaders)
        {
            return GetMetaData(current, outputRoot, shaders, TUKBinaryWriter.GetName(current.gameObject));
        }

        private static ModelData GetMetaData(Transform current, string outputRoot, ShaderData shaders, string modelName)
        {
            string meshPath = "";
            string materialPath = "";
            List<ModelData> children = new();

            if (current.TryGetComponent(out MeshFilter meshFilter))
            {
                Mesh mesh = meshFilter.sharedMesh;
                meshPath = TUKMeshExporter.GetRelativePath(mesh, modelName);
                TUKMeshExporter.Save(Path.Combine(outputRoot, meshPath), TUKMeshExporter.GetMetaData(mesh));
            }

            // 매시를 가지고 있으면 데이터를 저장한다.
            if (current.TryGetComponent(out Renderer renderer))
            {
                materialPath = TUKMaterialExporter.DirectoryPath + TUKBinaryWriter.GetName(renderer.sharedMaterial) + TUKMaterialExporter.FileExtension;
                TUKMaterialExporter.Save(Path.Combine(outputRoot, materialPath), TUKMaterialExporter.GetMetaData(renderer.sharedMaterial, shaders));
            }

            // 자식이 있으면 재귀적으로 데이터를 모아온다
            if (current.childCount > 0)
            {
                foreach (Transform child in current)
                {
                    children.Add(GetMetaData(child, outputRoot, shaders, modelName));
                }
            }

            ModelData modelData = new();

            modelData.Name = current.name;

            modelData.LocalPosition = current.localPosition;
            modelData.LocalRotation = current.localRotation;
            modelData.LocalScale = current.localScale;

            modelData.MeshPath = meshPath;
            modelData.MaterialPath = materialPath;

            modelData.ChildCount = children.Count;
            modelData.Children = children;

            return modelData;
        }
    }
}
