using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

using TMPro;
using UnityEditor;
using UnityEngine;
using UnityEngine.Rendering;
using Object = UnityEngine.Object;

namespace TUK.Editor
{
    internal struct FontAtlasData
    {
        internal int Index;
        internal int Width;
        internal int Height;
        internal string TexturePath;
        internal byte[] Pixels;
    }

    internal struct FontCharacterData
    {
        internal uint Unicode;
        internal uint GlyphIndex;
        internal int AtlasIndex;
        internal float CharacterScale;
        internal float GlyphScale;
        internal Vector4 GlyphRect;
        internal Vector4 SampleRect;
        internal Vector2 UVMin;
        internal Vector2 UVMax;
        internal float Width;
        internal float Height;
        internal float BearingX;
        internal float BearingY;
        internal float Advance;
    }

    internal struct FontData
    {
        internal string Name;
        internal string RenderMode;
        internal bool IsDistanceField;
        internal float PointSize;
        internal float FaceScale;
        internal float LineHeight;
        internal float AscentLine;
        internal float DescentLine;
        internal int AtlasPadding;
        internal List<FontAtlasData> Atlases;
        internal List<FontCharacterData> Characters;
    }

    internal static class TUKFontExporter
    {
        internal static readonly string DirectoryPath = "Resources/Fonts/";
        internal static readonly string FileExtension = ".tukfont";

        internal static void Save(string path, FontData data)
        {
            string directory = Path.GetDirectoryName(Path.GetFullPath(path));
            SaveAtlases(directory, data);

            using (TUKBinaryWriter writer = new(path))
            {
                Save(writer, data);
            }
        }

        internal static void SaveAtlases(string directory, FontData data)
        {
            Directory.CreateDirectory(directory);

            foreach (FontAtlasData atlas in data.Atlases)
            {
                File.WriteAllBytes(Path.Combine(directory, atlas.TexturePath), atlas.Pixels);
            }
        }

        internal static void Save(TUKBinaryWriter writer, FontData data)
        {
            writer.Write(data.Name);
            writer.Write(data.RenderMode);
            writer.Write(data.IsDistanceField);
            writer.Write(data.PointSize);
            writer.Write(data.FaceScale);
            writer.Write(data.LineHeight);
            writer.Write(data.AscentLine);
            writer.Write(data.DescentLine);
            writer.Write(data.AtlasPadding);
            
            writer.Write(data.Atlases.Count);

            foreach (FontAtlasData atlas in data.Atlases)
            {
                writer.Write(atlas.Index);
                writer.Write(atlas.TexturePath);
                writer.Write(atlas.Width);
                writer.Write(atlas.Height);
            }
            
            writer.Write(data.Characters.Count);

            foreach (FontCharacterData character in data.Characters)
            {
                writer.Write(character.Unicode);
                writer.Write(character.GlyphIndex);
                writer.Write(character.AtlasIndex);
                writer.Write(character.CharacterScale);
                writer.Write(character.GlyphScale);
                writer.Write(character.GlyphRect);
                writer.Write(character.SampleRect);
                writer.Write(character.UVMin);
                writer.Write(character.UVMax);
                writer.Write(character.Width);
                writer.Write(character.Height);
                writer.Write(character.BearingX);
                writer.Write(character.BearingY);
                writer.Write(character.Advance);
            }
        }

        internal static FontData GetMetaData(TMP_FontAsset current)
        {
            if (current.characterTable == null || 
                current.characterTable.Count == 0)
            {
                throw new InvalidOperationException("문자가 생성된 TMP 폰트가 필요합니다.");
            }

            var face = current.faceInfo;
            string renderMode = current.atlasRenderMode.ToString();

            FontData data = new();

            data.Name = current.name;
            data.RenderMode = renderMode;

            data.IsDistanceField = renderMode.IndexOf("SDF", StringComparison.OrdinalIgnoreCase) >= 0;

            data.Atlases = new();
            data.Characters = new();

            data.PointSize = face.pointSize;
            data.FaceScale = face.scale;
            data.LineHeight = face.lineHeight;
            data.AscentLine = face.ascentLine;
            data.DescentLine = face.descentLine;
            data.AtlasPadding = current.atlasPadding;
            var pages = new Dictionary<int, Texture2D>();

            foreach (TMP_Character character in current.characterTable.OrderBy(value => value == null ? 0u : value.unicode))
            {
                if (character == null || 
                    character.glyph == null)
                {
                    throw new InvalidOperationException("글리프가 없는 문자입니다.");
                }

                var glyph = character.glyph;
                int page = glyph.atlasIndex;

                if (current.atlasTextures == null || 
                    page < 0 || 
                    page >= current.atlasTextures.Length || 
                    !current.atlasTextures[page])
                {
                    throw new InvalidOperationException("글리프 아틀라스가 없습니다.");
                }

                Texture2D atlas = current.atlasTextures[page];
                pages[page] = atlas;
                var rect = glyph.glyphRect;

                if (rect.x < 0 || rect.y < 0 || rect.width < 0 || rect.height < 0 ||
                rect.x + rect.width > atlas.width || rect.y + rect.height > atlas.height)
                {
                    throw new InvalidOperationException("글리프 영역이 아틀라스 밖에 있습니다.");
                }

                int top = atlas.height - rect.y - rect.height;
                int padding = data.IsDistanceField && rect.width > 0 && rect.height > 0 ? data.AtlasPadding : 0;
                int left = Math.Max(0, rect.x - padding), right = Math.Min(atlas.width, rect.x + rect.width + padding);
                int sampleTop = Math.Max(0, top - padding), bottom = Math.Min(atlas.height, top + rect.height + padding);
                var metrics = glyph.metrics;

                FontCharacterData fontCharacterData = new();

                fontCharacterData.Unicode = character.unicode;
                fontCharacterData.GlyphIndex = glyph.index;
                fontCharacterData.AtlasIndex = page;

                fontCharacterData.CharacterScale = character.scale;
                fontCharacterData.GlyphScale = glyph.scale;

                fontCharacterData.GlyphRect = new Vector4(rect.x, top, rect.width, rect.height);
                fontCharacterData.SampleRect = new Vector4(left, sampleTop, right - left, bottom - sampleTop);

                fontCharacterData.UVMin = new Vector2((float)left / atlas.width, (float)sampleTop / atlas.height);
                fontCharacterData.UVMax = new Vector2((float)right / atlas.width, (float)bottom / atlas.height);

                fontCharacterData.Width = metrics.width;
                fontCharacterData.Height = metrics.height;
                fontCharacterData.BearingX = metrics.horizontalBearingX;
                fontCharacterData.BearingY = metrics.horizontalBearingY;
                fontCharacterData.Advance = metrics.horizontalAdvance;

                data.Characters.Add(fontCharacterData);
            }

            foreach (var page in pages.OrderBy(value => value.Key))
            {
                FontAtlasData fontAtlasData = new();

                fontAtlasData.Index = page.Key;

                fontAtlasData.Width = page.Value.width;
                fontAtlasData.Height = page.Value.height;

                fontAtlasData.TexturePath = TUKBinaryWriter.GetName(current) + "_" + page.Key + ".png";
                fontAtlasData.Pixels = EncodeAtlas(page.Value);

                data.Atlases.Add(fontAtlasData);
            }

            return data;
        }

        private static byte[] EncodeAtlas(Texture2D source)
        {
            if (source.format != TextureFormat.Alpha8 && source.format != TextureFormat.R8)
            {
                throw new InvalidOperationException("폰트 아틀라스는 Alpha8 또는 R8만 지원합니다.");
            }

            byte[] mask;

            if (source.isReadable)
            {
                mask = source.GetPixelData<byte>(0).ToArray();
            }
            else
            {
                if (!SystemInfo.supportsAsyncGPUReadback)
                {
                    throw new InvalidOperationException("아틀라스 Read/Write를 활성화하세요.");
                }

                var request = AsyncGPUReadback.Request(source, 0);
                request.WaitForCompletion();

                if (request.hasError)
                {
                    throw new InvalidOperationException("아틀라스 GPU 읽기에 실패했습니다.");
                }

                mask = request.GetData<byte>().ToArray();
            }

            if (mask.Length != source.width * source.height)
            {
                throw new InvalidOperationException("아틀라스 크기가 일치하지 않습니다.");
            }

            Texture2D output = new(source.width, source.height, TextureFormat.RGBA32, false, true);

            try
            {
                var colors = new Color32[mask.Length];

                for (int i = 0; i < mask.Length; i++)
                {
                    colors[i] = new Color32(mask[i], mask[i], mask[i], mask[i]);
                }

                output.SetPixels32(colors);
                output.Apply(false, false);
                byte[] bytes = output.EncodeToPNG();

                if (bytes == null || 
                    bytes.Length == 0)
                {
                    throw new InvalidOperationException("아틀라스 PNG 기록에 실패했습니다.");
                }

                return bytes;
            }
            finally
            {
                Object.DestroyImmediate(output);
            }
        }
    }
}
