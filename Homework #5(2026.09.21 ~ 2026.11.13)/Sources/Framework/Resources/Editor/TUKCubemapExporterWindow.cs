using System;

using UnityEditor;
using UnityEngine;

namespace TUK.Editor
{
    internal sealed class TUKCubemapExporterWindow : TUKExporterWindow
    {
        protected override Type ObjectType
        {
            get
            {
                return typeof(Cubemap);
            }
        }

        [MenuItem("Tools/TUK/Export Cubemap")]
        private static void Open()
        {
            OpenWindow<TUKCubemapExporterWindow>("TUK Cubemap Exporter");
        }
    }
}
