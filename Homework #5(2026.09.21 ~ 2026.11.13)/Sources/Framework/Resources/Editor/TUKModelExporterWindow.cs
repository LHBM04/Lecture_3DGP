using System;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKModelExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(GameObject);
            }
        }

        protected override bool AllowSceneObjects
        {
            get
            {
                return true;
            }
        }

        protected override bool ShowShaderPaths
        {
            get
            {
                return true;
            }
        }

        [MenuItem("Tools/TUK/Export Model")]
        private static void Open()
        {
            OpenWindow<TUKModelExporterWindow>("TUK Model Exporter");
        }
    }
}
