#if UNITY_EDITOR
using System;
using System.IO;

using UnityEngine;

using Object = UnityEngine.Object;

namespace TUK.Editor
{
    internal class TUKBinaryWriter : IDisposable
    {
        private readonly FileStream _Stream;
        private readonly BinaryWriter _Writer;

        internal TUKBinaryWriter(string path)
        {
            string fullPath = Path.GetFullPath(path);
            Directory.CreateDirectory(Path.GetDirectoryName(fullPath));

            _Stream = new(fullPath, FileMode.Create, FileAccess.Write);
            _Writer = new(_Stream);
        }

        public void Dispose()
        {
            _Writer.Dispose();

            Debug.Log($"[TUK] 파일 기록 종료: {_Stream.Name}");
        }

        internal static string GetName(Object asset)
        {
            return string.Join("_", asset.name.Split(Path.GetInvalidFileNameChars())).Trim().TrimEnd('.');
        }

        internal static string GetOutputRoot(string filePath)
        {
            DirectoryInfo directory = new FileInfo(Path.GetFullPath(filePath)).Directory;
            while (directory != null)
            {
                if (directory.Name == "Resources")
                {
                    return directory.Parent.FullName;
                }

                directory = directory.Parent;
            }

            return Path.GetDirectoryName(Path.GetFullPath(filePath));
        }

        internal void Write(int value)
        {
            _Writer.Write(value);
        }

        internal void Write(float value)
        {
            _Writer.Write(value);
        }

        internal void Write(bool value)
        {
            _Writer.Write(value);
        }

        internal void Write(string value)
        {
            _Writer.Write(value ?? "");
        }

        internal void Write(uint value)
        {
            _Writer.Write(value);
        }

        internal void Write(Color value)
        {
            _Writer.Write(value.r);
            _Writer.Write(value.g);
            _Writer.Write(value.b);
            _Writer.Write(value.a);
        }

        internal void Write(Vector2 value)
        {
            _Writer.Write(value.x);
            _Writer.Write(value.y);
        }

        internal void Write(Vector3 value)
        {
            _Writer.Write(value.x);
            _Writer.Write(value.y);
            _Writer.Write(value.z);
        }

        internal void Write(Vector4 value)
        {
            _Writer.Write(value.x);
            _Writer.Write(value.y);
            _Writer.Write(value.z);
            _Writer.Write(value.w);
        }

        internal void Write(Quaternion value)
        {
            _Writer.Write(value.x);
            _Writer.Write(value.y);
            _Writer.Write(value.z);
            _Writer.Write(value.w);
        }
    }
}
#endif
