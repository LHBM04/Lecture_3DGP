#if UNITY_EDITOR
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using TMPro;
using UnityEditor;
using UnityEngine;

using Object = UnityEngine.Object;

namespace TUK.Editor
{
    internal static class TUKExporter
    {
        internal static void Export(IEnumerable<Object> objects, string root, ShaderData shaders)
        {
            Object[] selectedObjects = objects.Where(value => value).Distinct().ToArray();
            if (selectedObjects.Length == 0)
            {
                Debug.LogWarning("내보낼 오브젝트나 에셋을 선택하세요.");
                return;
            }

            if (selectedObjects.Any(value => value is GameObject || value is Material) && !ValidateShaders(shaders))
            {
                return;
            }

            if (string.IsNullOrEmpty(root))
            {
                return;
            }

            HashSet<GameObject> selectedModels = new(selectedObjects.OfType<GameObject>());
            int savedCount = 0;

            foreach (Object selected in selectedObjects)
            {
                try
                {
                    if (selected is GameObject model && HasSelectedAncestor(model.transform, selectedModels))
                    {
                        continue;
                    }

                    if (ExportObject(root, selected, shaders))
                    {
                        savedCount++;
                    }
                }
                catch (Exception exception)
                {
                    Debug.LogError($"'{selected.name}' 내보내기 실패: {exception.Message}");
                    Debug.LogException(exception);
                }
            }

            AssetDatabase.Refresh();

            if (savedCount > 0)
            {
                Debug.Log($"리소스 {savedCount}개 저장 완료: {Path.GetFullPath(root)}");
            }
            else
            {
                Debug.LogWarning("저장된 리소스가 없습니다. Console 메시지를 확인하세요.");
            }
        }

        private static bool ValidateShaders(ShaderData data)
        {
            if (string.IsNullOrEmpty(data.VertexShader) || string.IsNullOrEmpty(data.PixelShader))
            {
                Debug.LogWarning("모델·머티리얼을 내보내려면 Vertex와 Pixel 셰이더 경로가 필요합니다.");
                return false;
            }

            string[] paths = { data.VertexShader, data.HullShader, data.DomainShader, data.GeometryShader, data.PixelShader };
            foreach (string path in paths)
            {
                if (string.IsNullOrEmpty(path))
                {
                    continue;
                }

                if (Path.IsPathRooted(path) || path.Contains(':') || path.Split('/').Contains("..") ||
                    !string.Equals(Path.GetExtension(path), ".cso", StringComparison.OrdinalIgnoreCase))
                {
                    Debug.LogWarning($"셰이더 경로는 출력 리소스 기준의 CSO 상대 경로로 입력하세요: {path}");
                    return false;
                }
            }

            return true;
        }

        private static bool HasSelectedAncestor(Transform current, HashSet<GameObject> selectedModels)
        {
            for (Transform parent = current.parent; parent; parent = parent.parent)
            {
                if (selectedModels.Contains(parent.gameObject))
                {
                    return true;
                }
            }

            return false;
        }

        private static bool ExportObject(string root, Object selected, ShaderData shaders)
        {
            string name = TUKBinaryWriter.GetName(selected);

            switch (selected)
            {
                case GameObject model:
                {
                    ModelData data = TUKModelExporter.GetMetaData(model.transform, root, shaders);
                    string path = Path.Combine(root, TUKModelExporter.DirectoryPath, name + TUKModelExporter.FileExtension);

                    using (TUKBinaryWriter writer = new(path))
                    {
                        TUKModelExporter.Save(writer, data);
                    }

                    return true;
                }
                case Mesh mesh:
                {
                    MeshData data = TUKMeshExporter.GetMetaData(mesh);
                    string path = Path.Combine(root, TUKMeshExporter.GetRelativePath(mesh));

                    TUKMeshExporter.Save(path, data);
                    return true;
                }
                case Material material:
                {
                    MaterialData data = TUKMaterialExporter.GetMetaData(material, shaders);
                    string path = Path.Combine(root, TUKMaterialExporter.DirectoryPath, name + TUKMaterialExporter.FileExtension);

                    TUKMaterialExporter.Save(path, data);
                    return true;
                }
                case TMP_FontAsset font:
                {
                    FontData data = TUKFontExporter.GetMetaData(font);
                    string path = Path.Combine(root, TUKFontExporter.DirectoryPath, name + TUKFontExporter.FileExtension);

                    TUKFontExporter.Save(path, data);
                    return true;
                }
                case AnimationClip clip:
                {
                    AnimationData data = TUKAnimationExporter.GetMetaData(clip);
                    if (data.Transforms == null)
                    {
                        return false;
                    }

                    string path = Path.Combine(root, TUKAnimationExporter.DirectoryPath, name + TUKAnimationExporter.FileExtension);

                    TUKAnimationExporter.Save(path, data);
                    return true;
                }
                case Cubemap cubemap:
                {
                    CubemapData data = TUKCubemapExporter.GetMetaData(cubemap);
                    if (data.Faces == null)
                    {
                        return false;
                    }

                    TUKCubemapExporter.Save(Path.Combine(root, TUKCubemapExporter.DirectoryPath, name), data);
                    return true;
                }
                default:
                {
                    Debug.LogWarning($"지원하지 않는 선택 타입입니다: {selected.GetType().Name}");
                    return false;
                }
            }
        }
    }
}
#endif
