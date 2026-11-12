#if UNITY_EDITOR
using System;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKMeshExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(Mesh);
            }
        }

        [MenuItem("Tools/TUK/Export Mesh")]
        private static void Open()
        {
            OpenWindow<TUKMeshExporterWindow>("TUK Mesh Exporter");
        }
    }
}
#endif
