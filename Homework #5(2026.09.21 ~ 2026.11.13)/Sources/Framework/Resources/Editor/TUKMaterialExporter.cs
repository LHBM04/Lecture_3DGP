using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using UnityEditor;
using UnityEngine;

using PackageInfo = UnityEditor.PackageManager.PackageInfo;

namespace TUK.Editor
{
    internal struct MaterialData
    {
        internal Color BaseColor;
        internal Color EmissiveColor;

        internal float Metallic;
        internal float Roughness;

        internal string BaseColorTexture;
        internal string NormalTexture;
        internal string MetallicTexture;
        internal string RoughnessTexture;
        internal string OcclusionTexture;
        internal string EmissiveTexture;
        internal string HeightTexture;
        internal string SpecularTexture;
        internal string MaskTexture;

        internal string VertexShader;
        internal string HullShader;
        internal string DomainShader;
        internal string GeometryShader;
        internal string PixelShader;

        internal Dictionary<string, string> TextureSources;
    }

    internal static class TUKMaterialExporter
    {
        internal static readonly string DirectoryPath = "Resources/Materials/";
        internal static readonly string FileExtension = ".tukmat";

        private static readonly string[] AvailableExtensions =
        {
            ".png",
            ".jpg",
            ".jpeg",
            ".tga",
            ".tif",
            ".tiff",
            ".psd",
            ".exr",
            ".hdr",
            ".bmp",
            ".dds"
        };

        internal static void Save(string path, MaterialData data)
        {
            SaveTextures(TUKBinaryWriter.GetOutputRoot(path), data);
            using (TUKBinaryWriter writer = new(path))
            {
                Save(writer, data);
            }
        }

        internal static void SaveTextures(string root, MaterialData data)
        {
            foreach (var texture in data.TextureSources)
            {
                string destination = Path.GetFullPath(Path.Combine(root, texture.Key));
                Directory.CreateDirectory(Path.GetDirectoryName(destination));

                if (!string.Equals(destination, texture.Value, StringComparison.OrdinalIgnoreCase))
                {
                    File.Copy(texture.Value, destination, true);
                }
            }
        }

        internal static void Save(TUKBinaryWriter writer, MaterialData data)
        {
            writer.Write(data.BaseColor);
            writer.Write(data.EmissiveColor);
            writer.Write(data.Metallic);
            writer.Write(data.Roughness);

            writer.Write(data.BaseColorTexture);
            writer.Write(data.NormalTexture);
            writer.Write(data.MetallicTexture);
            writer.Write(data.RoughnessTexture);
            writer.Write(data.OcclusionTexture);
            writer.Write(data.EmissiveTexture);
            writer.Write(data.HeightTexture);
            writer.Write(data.SpecularTexture);
            writer.Write(data.MaskTexture);

            writer.Write(data.VertexShader);
            writer.Write(data.HullShader);
            writer.Write(data.DomainShader);
            writer.Write(data.GeometryShader);
            writer.Write(data.PixelShader);
        }

        private static string GetTexture(Material material, Dictionary<string, string> sources, params string[] properties)
        {
            foreach (string property in properties)
            {
                if (!material.HasProperty(property))
                {
                    continue;
                }

                Texture texture = material.GetTexture(property);
                if (!texture)
                {
                    continue;
                }

                string assetPath = AssetDatabase.GetAssetPath(texture);
                if (!(texture is Texture2D) || string.IsNullOrEmpty(assetPath))
                {
                    throw new InvalidOperationException($"{material.name}: {property} 속성에는 외부 이미지 Texture2D가 필요합니다.");
                }

                var package = PackageInfo.FindForAssetPath(assetPath);
                string source = package == null ? Path.GetFullPath(assetPath) :
                Path.Combine(package.resolvedPath, assetPath.Substring(package.assetPath.Length).TrimStart('/'));
                
                string extension = Path.GetExtension(source).ToLowerInvariant();
                if (!AvailableExtensions.Contains(extension) || !File.Exists(source))
                {
                    throw new InvalidOperationException($"원본 텍스처 이미지가 필요합니다: {assetPath}");
                }

                string normalizedAssetPath = assetPath.Replace('\\', '/');
                int texturesIndex = normalizedAssetPath.LastIndexOf("/Textures/", StringComparison.OrdinalIgnoreCase);
                string textureDirectory = "";

                if (texturesIndex >= 0)
                {
                    string texturePath = normalizedAssetPath.Substring(texturesIndex + "/Textures/".Length);
                    textureDirectory = Path.GetDirectoryName(texturePath)?.Replace('\\', '/') ?? "";
                }

                if (string.IsNullOrEmpty(textureDirectory))
                {
                    textureDirectory = TUKBinaryWriter.GetName(material);
                }

                string fileName = TUKBinaryWriter.GetName(texture) + extension;
                string relative = $"Resources/Textures/{textureDirectory}/{fileName}";
                sources[relative] = source;

                return relative;
            }

            return "";
        }

        internal static MaterialData GetMetaData(Material current)
        {
            return GetMetaData(current, "Shaders/Standard.vert.cso", "", "", "", "Shaders/Standard.frag.cso");
        }

        internal static MaterialData GetMetaData(
            Material current,
            string vertexShader, string hullShader, string domainShader, string geometryShader, string pixelShader)
        {
            Color baseColor = Color.white;
            Color emissiveColor = Color.black;
            float metallic = 0f;
            float roughness = 0.5f;

            Dictionary<string, string> sources = new();

            if (current.HasProperty("_BaseColor"))
            {
                baseColor = current.GetColor("_BaseColor");
            }
            else if (current.HasProperty("_Color"))
            {
                baseColor = current.GetColor("_Color");
            }
            if (current.HasProperty("_EmissionColor"))
            {
                emissiveColor = current.GetColor("_EmissionColor");
            }

            if (current.HasProperty("_Metallic"))
            {
                metallic = Mathf.Clamp01(current.GetFloat("_Metallic"));
            }

            if (current.HasProperty("_Roughness"))
            {
                roughness = Mathf.Clamp01(current.GetFloat("_Roughness"));
            }
            else if (current.HasProperty("_Smoothness"))
            {
                roughness = 1f - Mathf.Clamp01(current.GetFloat("_Smoothness"));
            }
            else if (current.HasProperty("_Glossiness"))
            {
                roughness = 1f - Mathf.Clamp01(current.GetFloat("_Glossiness"));
            }

            MaterialData materialData = new();

            materialData.BaseColor = baseColor;
            materialData.EmissiveColor = emissiveColor;

            materialData.Metallic = metallic;
            materialData.Roughness = roughness;

            materialData.BaseColorTexture = GetTexture(current, sources, "_BaseMap", "_BaseColorMap", "_MainTex");
            materialData.NormalTexture = GetTexture(current, sources, "_BumpMap", "_NormalMap");
            materialData.MetallicTexture = GetTexture(current, sources, "_MetallicGlossMap", "_MetallicMap");
            materialData.RoughnessTexture = GetTexture(current, sources, "_RoughnessMap");
            materialData.OcclusionTexture = GetTexture(current, sources, "_OcclusionMap", "_AmbientOcclusionMap");
            materialData.EmissiveTexture = GetTexture(current, sources, "_EmissionMap", "_EmissiveColorMap");
            materialData.HeightTexture = GetTexture(current, sources, "_ParallaxMap", "_HeightMap");
            materialData.SpecularTexture = GetTexture(current, sources, "_SpecGlossMap", "_SpecularColorMap");
            materialData.MaskTexture = GetTexture(current, sources, "_MaskMap");

            materialData.VertexShader = vertexShader;
            materialData.HullShader = hullShader;
            materialData.DomainShader = domainShader;
            materialData.GeometryShader = geometryShader;
            materialData.PixelShader = pixelShader;

            materialData.TextureSources = sources;

            return materialData;
        }
    }
}
