#if UNITY_EDITOR
using System;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKMaterialExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(Material);
            }
        }

        protected override bool ShowShaderPaths
        {
            get
            {
                return true;
            }
        }

        [MenuItem("Tools/TUK/Export Material")]
        private static void Open()
        {
            OpenWindow<TUKMaterialExporterWindow>("TUK Material Exporter");
        }
    }
}
#endif
